#include "TransparentMainWindow.h"
#include "SingleApplication.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QDesktopServices>
#include <QClipboard>
#include <QMimeData>

int main(int argc,  char**argv)
{
	//qputenv("QT_SCALE_FACTOR", "0.8");
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	SingleApplication a(argc, argv);
	
	if (a.instanceRunning())
		return 0;

	auto appIcon = QIcon(":/icon/MainIcon");

	QMainWindow mainWindow;
	
	qApp->setWindowIcon(appIcon);
	QSystemTrayIcon systemTray;
	systemTray.setIcon(appIcon);

	std::atomic<bool> clipboardUpdated{ true };
	QObject::connect(qApp->clipboard(), &QClipboard::dataChanged, [&]
		{
			clipboardUpdated = true;
		});
	std::function<TransparentMainWindow* (QByteArray)> createPinWindow;
	std::function<TransparentMainWindow* (QString)> createPinWindowByFile;
	std::function<TransparentMainWindow* (QImage)> createPinWindowByImage;
	std::function<void (const QMimeData*)> createPinWindowByMimeData;
	
	createPinWindowByMimeData = [&](const QMimeData* mimeData)
	{
		bool success = false;
		if (mimeData->hasUrls())
			for (auto url : mimeData->urls())
				if (url.isLocalFile())
					if (createPinWindowByFile(url.toLocalFile()) != nullptr)
						success = true;
		if (!success && mimeData->hasImage())
		{
			createPinWindowByImage(qvariant_cast<QImage>(mimeData->imageData()));
		}
	};

	createPinWindow = [&](QByteArray data)->TransparentMainWindow*
	{
		auto w = new TransparentMainWindow(&mainWindow);
		w->show();
		if (!w->loadMovie(data))
		{
			w->deleteLater();
			return nullptr;
		}
		QObject::connect(w, static_cast<void(TransparentMainWindow::*)(QByteArray)const>(&TransparentMainWindow::cloneWindow), createPinWindow);
		QObject::connect(w, static_cast<void(TransparentMainWindow::*)(const QMimeData*)const>(&TransparentMainWindow::cloneWindow), createPinWindowByMimeData);
		return w;
	};
	createPinWindowByFile = [&](QString fileName)->TransparentMainWindow*
	{
		auto w = new TransparentMainWindow(&mainWindow);
		w->show();
		if(!w->loadMovie(fileName))
		{
			w->deleteLater();
			return nullptr;
		}
		
		QObject::connect(w, static_cast<void(TransparentMainWindow::*)(QByteArray)const>(&TransparentMainWindow::cloneWindow), createPinWindow);
		QObject::connect(w, static_cast<void(TransparentMainWindow::*)(const QMimeData*)const>(&TransparentMainWindow::cloneWindow), createPinWindowByMimeData);
		return w;
	};
	createPinWindowByImage = [&](QImage image)
	{
		auto w = new TransparentMainWindow(&mainWindow);
		w->show();
		w->loadMovie(image);
		QObject::connect(w, static_cast<void(TransparentMainWindow::*)(QByteArray)const>(&TransparentMainWindow::cloneWindow), createPinWindow);
		QObject::connect(w, static_cast<void(TransparentMainWindow::*)(const QMimeData*)const>(&TransparentMainWindow::cloneWindow), createPinWindowByMimeData);
		return w;
	};
	

	auto parserCommandLineToCreateWindow = [&](QStringList commandLine)
	{
		const QCommandLineOption inputImageOption("i", QObject::tr("Input Image File Path"), "ImagePath");
		const QCommandLineOption clipboardImageOption("c", QObject::tr("Use Clipboard"));
		QCommandLineParser parser;
		parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
		parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
		parser.addOption(inputImageOption);
		parser.addOption(clipboardImageOption);
		parser.process(commandLine);

		bool success{ false };
		if (parser.isSet(inputImageOption))
		{
			createPinWindowByFile(parser.value(inputImageOption));
			success = true;
		}
		if (parser.isSet(clipboardImageOption))
		{
			createPinWindowByMimeData(qApp->clipboard()->mimeData());
			success = true;
		}

		return success;
	};

	
	QObject::connect(&a, &SingleApplication::newInstanceStartup, [&](QStringList commandLine)
		{
			if (parserCommandLineToCreateWindow(commandLine))
				return;
		
			if (clipboardUpdated)
			{
				clipboardUpdated = false;
				createPinWindowByMimeData(qApp->clipboard()->mimeData());
			}
			else
			{
				systemTray.showMessage(QObject::tr("Invalid parameter"), commandLine.join(' '), QIcon(":/icon/res/button/error.png"));
			}
		});


	QMenu systemTrayMenu;
	
	systemTrayMenu.addAction(QIcon(":/icon/res/button/folder.png"), "&Open", [&]
		{
			if (auto fileNames = QFileDialog::getOpenFileNames(nullptr, "Load image", ""
				, "All Support Image Files(*.bmp;*.jpg;*.jpeg;*.png;*.gif);;Static Images Files(*.bmp;*.jpg;*.jpeg;*.png);;Dynamic Image Files(*.gif);;All Files(*.*)");
				!fileNames.isEmpty())
				for (auto fileName : fileNames)
					createPinWindowByFile(fileName);
		});

	systemTrayMenu.addAction(QIcon(":/icon/res/button/clipboard.png"), "&From Clipboard", [&]
		{
			createPinWindowByMimeData(qApp->clipboard()->mimeData());
		});

	systemTrayMenu.addAction(QIcon(":/icon/res/button/close.png"), "&Close All", [&]
		{
			auto& children = mainWindow.children();

			for (auto children : children)
			{
				if (auto window = dynamic_cast<TransparentMainWindow*>(children))
				{
					window->close();
				}
			}
		});

	systemTrayMenu.addAction(QIcon(":/icon/res/button/demo.png"), "&Demo", [=]
		{
			createPinWindowByFile(":/icon/Demo");
		});

	systemTrayMenu.addAction(QIcon(":/icon/res/button/about.png"), "&About", [=]
		{
			QDesktopServices::openUrl(QUrl("https://github.com/Ohto-Ai"));
		});

	systemTrayMenu.addAction(QIcon(":/icon/res/button/quit.png"), "&Quit", [=]
		{
			if (QMessageBox::question(nullptr, QObject::tr("Quit"), QObject::tr("Quit the application?")) == QMessageBox::Yes)
				qApp->quit();
		});

	systemTray.setContextMenu(&systemTrayMenu);
	
	systemTray.show();
	
	qApp->setQuitOnLastWindowClosed(false);

	return a.exec();
}