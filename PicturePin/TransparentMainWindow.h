#pragma once
#include <QtWidgets/QMainWindow>
#include <QMimeData>
#include <QPainter>
#include <QAction>
#include <QMovie>
#include <QLabel>
#include <QStyle>
#include <QTimer>
#include <QFileDialog>
#include <QBuffer>
#include <QApplication>
#include <QDropEvent>
#include <QClipboard>
#include <QScreen>

class TransparentMainWindow :
    public QMainWindow
{
	Q_OBJECT
public:
	TransparentMainWindow(QWidget* parent = nullptr)
		: QMainWindow{parent}
	{
		setCentralWidget(&mainLabel);
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
		setAttribute(Qt::WA_TranslucentBackground);
		mainLabel.setAlignment(Qt::AlignCenter);
		mainLabel.setMovie(&windowMovie);

		windowMovie.setCacheMode(QMovie::CacheAll);
				
		auto loadMovieAC = new QAction(QIcon(":/icon/res/button/folder.png"), tr("[&O]pen Image"), this);
		loadMovieAC->setShortcut(QKeySequence{ "Ctrl+O" });

		auto captureCurrentImage = new QAction(QIcon(":/icon/res/button/capture.png"), tr("[&C]apture current Image"), this);
		captureCurrentImage->setShortcut(QKeySequence{ "Ctrl+C" });
		
		auto lockPositionAC = new QAction(QIcon(":/icon/res/button/lock.png"), tr("Lock [&P]osition"), this);
		lockPositionAC->setCheckable(true);
		lockPositionAC->setShortcut(QKeySequence{ "Ctrl+P" });
		
		auto moveCenterAC = new QAction(QIcon(":/icon/res/button/cross.png"), tr("[&M]ove Center"), this);
		moveCenterAC->setShortcut(QKeySequence{ "Ctrl+M" });

		auto closeAC = new QAction(QIcon(":/icon/res/button/close.png"), tr("Close"), this);
		closeAC->setShortcut(QKeySequence{ "Alt+F4" });

		auto cloneWindowAC = new QAction(QIcon(":/icon/res/button/clone.png"), tr("Clone [&W]indow"), this);
		cloneWindowAC->setShortcut(QKeySequence{ "Alt+W" });
					
		connect(closeAC, &QAction::triggered, this, &QMainWindow::close);
				
		connect(loadMovieAC, &QAction::triggered, [=]
		{
				loadMovie(QFileDialog::getOpenFileName(this, "Load image", ""
					, "All Support Image Files(*.bmp;*.jpg;*.jpeg;*.png;*.gif);;Static Images Files(*.bmp;*.jpg;*.jpeg;*.png);;Dynamic Image Files(*.gif);;All Files(*.*)"));
		});

		connect(captureCurrentImage, &QAction::triggered, [=]
			{
				qApp->clipboard()->setImage(windowMovie.currentImage());
			});
		
		connect(moveCenterAC, &QAction::triggered, [=]
			{
				move((QApplication::primaryScreen()->size().width() - width()) / 2, (QApplication::primaryScreen()->size().height() - height()) / 2);
			});
				
		connect(lockPositionAC, &QAction::triggered, [=]
			{
				isPositionLocked = lockPositionAC->isChecked();
			});
		
		connect(cloneWindowAC, &QAction::triggered, [=]
			{
				emit cloneWindow(imageByteArray);
			});
		
		addAction(loadMovieAC);
		addAction(captureCurrentImage);
		addAction(moveCenterAC);
		addAction(lockPositionAC);
		addAction(cloneWindowAC);
		addAction(closeAC);
		setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

		setAcceptDrops(true);
	}

	void doLoadMovie()
	{
		deviceBuffer.open(QIODevice::ReadOnly);
		windowMovie.setDevice(&deviceBuffer);
		windowMovie.start();
		setFixedSize(windowMovie.scaledSize());
		move((QApplication::primaryScreen()->size().width() - width()) / 2, (QApplication::primaryScreen()->size().height() - height()) / 2);
	}

	bool loadMovie(QByteArray data)
	{
		deviceBuffer.close();
		deviceBuffer.open(QIODevice::WriteOnly);
		deviceBuffer.write(data);
		deviceBuffer.close();
		doLoadMovie();
		return true;
	}
	
	bool loadMovie(QImage image)
	{
		deviceBuffer.close();
		deviceBuffer.open(QIODevice::WriteOnly);
		image.save(&deviceBuffer, "png");
		deviceBuffer.close();
		doLoadMovie();
		return true;
	}
	
	bool loadMovie(QString fileName)
	{
		if (fileName.isEmpty())
			return false;
		if (const QMovie movie(fileName); !movie.isValid())
			return false;

		QFile file(fileName);
		deviceBuffer.close();
		deviceBuffer.open(QIODevice::WriteOnly);
		file.open(QIODevice::ReadOnly);
		deviceBuffer.write(file.readAll());
		deviceBuffer.close();
		file.close();
		
		doLoadMovie();
		return true;
	}

	void dragEnterEvent(QDragEnterEvent* event) override
	{
		auto mimeData = event->mimeData();
		bool success = false;
		if (mimeData->hasUrls())
		{
			for (auto url : mimeData->urls())
			{
				if (url.isLocalFile())
				{
					const QMovie movie(url.toLocalFile());
					if (movie.isValid())
					{
						success = true;
						break;
					}
				}
			}
		}
		if (!success && mimeData->hasImage())
		{
			success = true;
		}
		if (success)
			event->acceptProposedAction();
	}
	void dropEvent(QDropEvent* event) override
	{
		emit cloneWindow(event->mimeData());
	}
	
	void mousePressEvent(QMouseEvent* event) override
	{
		if (event->button() == Qt::LeftButton)
		{
			onDragging = true;
			startPosition = event->globalPos();
			framePosition = frameGeometry().topLeft();
			setCursor(Qt::ClosedHandCursor);
		}
		QMainWindow::mousePressEvent(event);
	}
	void mouseMoveEvent(QMouseEvent* event) override
	{
		if (event->buttons() & Qt::LeftButton)
		{
			if (onDragging && !isPositionLocked)
			{
				QPoint delta = event->globalPos() - startPosition;
				move(framePosition + delta);
			}
		}
		QMainWindow::mouseMoveEvent(event);
	}
	void mouseReleaseEvent(QMouseEvent* event) override
	{
		onDragging = false;
		setCursor(Qt::ArrowCursor);
		QMainWindow::mouseReleaseEvent(event);
	}

	void closeEvent(QCloseEvent* event) override
	{
		deleteLater();
		QMainWindow::closeEvent(event);
	}
	
signals:
	void cloneWindow(QByteArray) const;
	void cloneWindow(const QMimeData*) const;
private:
	bool onDragging;		// 是否正在拖动
	QPoint startPosition;	// 拖动开始前的鼠标位置
	QPoint framePosition;	// 窗体的原始位置

	bool isPositionLocked{ false };
	QLabel mainLabel;
	QMovie windowMovie{ this };

	QByteArray imageByteArray;
	QBuffer deviceBuffer{ &imageByteArray, this };
};

