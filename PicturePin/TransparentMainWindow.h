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

class TransparentMainWindow :
    public QMainWindow
{
	Q_OBJECT
public:
	TransparentMainWindow(QWidget* parent = nullptr)
		: QMainWindow{parent}
		, patternLength{ 4 }
	{
		setWindowIcon(QIcon(":/icon/pin.png"));
		setCentralWidget(&mainLabel);
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
		setAttribute(Qt::WA_TranslucentBackground);
		mainLabel.setAlignment(Qt::AlignCenter);
		mainLabel.setMovie(&windowMovie);

		windowMovie.setCacheMode(QMovie::CacheAll);

		dashes = patternLength;
		spaces = patternLength;
		dashPattern.clear();
		for (int i = 0; i < 4; ++i) {
			dashPattern << patternLength;
		}

		repaintTimer = new QTimer();
		repaintTimer->setInterval(100);
		connect(repaintTimer, &QTimer::timeout, this, &TransparentMainWindow::updateAntLineValue);
		
		auto showAntLineAC = new QAction(tr("[&B]order"), this);
		showAntLineAC->setCheckable(true);
		showAntLineAC->setShortcut(QKeySequence{ "Ctrl+B" });
		
		auto loadMovieAC = new QAction(style()->standardIcon(QStyle::StandardPixmap::SP_DialogOpenButton), tr("[&O]pen Image"), this);
		loadMovieAC->setShortcut(QKeySequence{ "Ctrl+O" });
		
		auto lockPositionAC = new QAction(tr("Lock [&P]osition"), this);
		lockPositionAC->setCheckable(true);
		lockPositionAC->setShortcut(QKeySequence{ "Ctrl+P" });
		
		auto moveCenterAC = new QAction(tr("[&M]ove Center"), this);
		moveCenterAC->setShortcut(QKeySequence{ "Ctrl+M" });

		auto closeAC = new QAction(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton), tr("Close"), this);
		closeAC->setShortcut(QKeySequence{ "Alt+F4" });

		auto cloneWindowAC = new QAction(tr("Clone [&W]indow"), this);
		cloneWindowAC->setShortcut(QKeySequence{ "Alt+W" });
					
		connect(closeAC, &QAction::triggered, this, &QMainWindow::close);
				
		connect(loadMovieAC, &QAction::triggered, [=]
		{
				loadMovie(QFileDialog::getOpenFileName(this, "Load image", ""
					, "All Support Image Files(*.bmp;*.jpg;*.jpeg;*.png;*.gif);;Static Images Files(*.bmp;*.jpg;*.jpeg;*.png);;Dynamic Image Files(*.gif);;All Files(*.*)"));
		});
		
		connect(moveCenterAC, &QAction::triggered, [=]
			{
				move((QApplication::primaryScreen()->size().width() - width()) / 2, (QApplication::primaryScreen()->size().height() - height()) / 2);
			});
		
		connect(showAntLineAC, &QAction::triggered, [=]
			{
				isBorderShown = showAntLineAC->isChecked();
				if (isBorderShown)
					repaintTimer->start();
				else
					repaintTimer->stop();
				update();
			});
		
		connect(lockPositionAC, &QAction::triggered, [=]
			{
				isPositionLocked = lockPositionAC->isChecked();
			});
		
		connect(cloneWindowAC, &QAction::triggered, [=]
			{
				emit cloneWindow(imageByteArray);
			});
		addAction(showAntLineAC);	
		addAction(loadMovieAC);
		addAction(moveCenterAC);
		addAction(lockPositionAC);
		addAction(cloneWindowAC);
		addAction(closeAC);
		setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

		setAcceptDrops(true);
	}

	void updateAntLineValue()
	{
		if (isBorderShown)
		{
			//当蚂蚁线走到末尾,则重新赋值
			if (dashes == patternLength && spaces == patternLength) {
				dashes = 0;
				spaces = 0;
			}

			if (dashes == 0 && spaces < patternLength) {
				++spaces;
			}
			else if (spaces == patternLength && dashes < patternLength) {
				++dashes;
			}

			//每次只需要将前面两个长度更新就行
			dashPattern[0] = dashes;
			dashPattern[1] = spaces;

			update(0, 0, mainLabel.width(), 2);
			update(0, mainLabel.height() - 3, mainLabel.width(), 2);
			update(0, 0, 2, mainLabel.height());
			update(mainLabel.width() - 3, 0, 2, mainLabel.height());
		}
	}
	
	void paintEvent(QPaintEvent* event) override
	{
		if (isBorderShown)
		{
			QPainter painter(this);
			painter.setRenderHints(QPainter::Antialiasing);

			QPen pen;
			pen.setWidth(2);
			pen.setColor(Qt::white);
			pen.setDashPattern(dashPattern);
			painter.setPen(pen);
			painter.drawRect(mainLabel.rect().adjusted(0, 0, -1, -1));

			auto rect = mainLabel.rect().width();
		}
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
			startPosition = event->globalPosition().toPoint();
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
				QPoint delta = event->globalPosition().toPoint() - startPosition;
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

	bool isBorderShown{ false };
	QTimer* repaintTimer;
	int dashes;
	int spaces;
	const int patternLength;
	QVector<qreal> dashPattern;

	QByteArray imageByteArray;
	QBuffer deviceBuffer{ &imageByteArray, this };
};

