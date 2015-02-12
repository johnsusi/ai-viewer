#include <chrono>

class Timer {

public:
  using clock = std::chrono::steady_clock;

  template <typename T>
  std::size_t time() const {
    return std::chrono::duration_cast<T>(clock::now() - start).count();
  }

  std::size_t nanoseconds() const { return time<std::chrono::nanoseconds>(); }
  std::size_t microseconds() const { return time<std::chrono::microseconds>(); }
  std::size_t milliseconds() const { return time<std::chrono::milliseconds>(); }
  std::size_t seconds() const { return time<std::chrono::seconds>(); }
  std::size_t minutes() const { return time<std::chrono::minutes>(); }
  std::size_t hours() const { return time<std::chrono::hours>(); }


private:

  clock::time_point start = clock::now();

};

#include <QApplication>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QObjectCleanupHandler>
#include <QPixmap>
#include <QResource>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWebFrame>
#include <QWebPage>
#include <QWebSettings>
#include <QWebView>
#include <QWebPluginFactory>


#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>


auto convertToMat(const QImage& source) {

  return cv::Mat(source.height(),source.width(),CV_8UC4,(uchar*)source.bits(),source.bytesPerLine());
  // cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
  // cv::cvtColor(tmp, result, CV_RGBA2BGR);
  // return result;
}



auto convertToQImage(const cv::Mat& source) {


  auto color_table = [] {
    static auto table = [] {
      QVector<QRgb> table;
      for (int i = 0;i < 256;++i) table.push_back( qRgb(i,i,i) );
      return table;
    }();
    return table;
  };
  int step = source.step;
  switch (source.type()) {
  case CV_8UC4: return QImage{source.data, source.cols, source.rows, step, QImage::Format_RGB32};
  case CV_8UC3: return QImage{source.data, source.cols, source.rows, step, QImage::Format_RGB888};
  case CV_8UC1: {
    QImage image{source.data, source.cols, source.rows, step, QImage::Format_Indexed8};
    image.setColorTable(color_table());
    return image;
  }
  default:
    qWarning() << "Cannot convert OpenCV Mat to QImage";
    return QImage();
  }
}

class ImageViewer : public QLabel {
  Q_OBJECT
public:

  template <typename T> T get(const QVariant& value, T default_value) {
    if (value.isValid()) return value.value<T>();
    return default_value;
  }

  ImageViewer(QUrl url, QVariantMap params, QWidget* parent = 0) : QLabel(parent) {

    QNetworkRequest request(url);
    auto reply = network_access_manager.get(request);

    req_width = get(params["width"], 0);
    req_height = get(params["height"], 0);

    connect(reply, &QNetworkReply::finished, [=] {
      raw_image.loadFromData( reply->readAll() );
      reply->deleteLater();

      scaleImage(raw_image);
    });

  }

public slots:

  void threshold() {
    qDebug() << "threshold";
    Timer t;
    auto tmp = convertToMat(raw_image);
    cv::cvtColor(tmp, tmp, cv::CV_BGR2GRAY);
    tmp = tmp > 128;
    cv::resize(tmp, tmp, {500, 500});
    auto img = convertToQImage(tmp);
    qDebug() << "time:" << t.milliseconds() << "ms";
    scaleImage( img );

  }

  void blur() {
    Timer t;
    auto tmp = convertToMat(raw_image);
    cv::GaussianBlur(tmp, tmp, cv::Size(3,3), 0, 0);
//    cv::resize(tmp, tmp, {500, 500});
    auto img = convertToQImage(tmp);
    qDebug() << "time:" << t.milliseconds() << "ms";
    scaleImage( img );

  }


  void grayscale() {
    Timer t;
    auto tmp = convertToMat(raw_image);
    cv::cvtColor(tmp, tmp, cv::CV_BGR2GRAY);
//    cv::resize(tmp, tmp, {500, 500});
    auto img = convertToQImage(tmp);
    qDebug() << "time:" << t.milliseconds() << "ms";
    scaleImage( img );
  }

  void sharpen() {
    Timer t;
    auto tmp = convertToMat(raw_image);
    cv::Mat mask;
    cv::GaussianBlur(tmp, mask, cv::Size(0, 0), 3);
    cv::addWeighted(tmp, 1.5, mask, -0.5, 0, tmp);
//    cv::resize(tmp, tmp, {500, 500});
    auto img = convertToQImage(tmp);
    qDebug() << "time:" << t.milliseconds() << "ms";
    scaleImage( img );
  }

  void scaleImage(const QImage& input) {

      if (req_width > 0 && req_height == 0)      cur_image = input.scaledToWidth(req_width, Qt::SmoothTransformation);
      else if (req_width == 0 && req_height > 0) cur_image = input.scaledToHeight(req_height, Qt::SmoothTransformation);
      else if (req_width > 0 && req_height > 0)  cur_image = input.scaled(req_width, req_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
      else cur_image = input;


//      setFixedSize(std::min(req_width, cur_image.width()),
//                   std::min(req_height, cur_image.height()));

      qDebug() << input.width() << input.height() << width() << height() << cur_image.width() << cur_image.height();
      setPixmap(QPixmap::fromImage(cur_image));
      setScaledContents(true);
//      updateGeometry();
  }
private:

  QImage raw_image, cur_image;
  cv::Mat cv_image;

  int req_width, req_height;
  QNetworkAccessManager network_access_manager;
};

class WebPage : public QWebPage {
  Q_OBJECT
public:

  WebPage(QObject* parent = 0) : QWebPage(parent) {
    setLinkDelegationPolicy(DelegateAllLinks);
  }

  QObject* createPlugin(
      const QString & classid,
      const QUrl & url,
      const QStringList & paramNames,
      const QStringList & paramValues) {

    qDebug() << url;

    QVariantMap params;

    for (int i = 0;i < paramNames.size();++i) {
      params.insert(paramNames[i], paramValues[i]);
    }


    if (classid == "se.susi.Filter") {
      auto result = new ImageViewer(url, std::move(params));
//      cleanup_handler.add(result);
      return result;
    }

    return QWebPage::createPlugin(classid, url, paramNames, paramValues);
  }
private:

  QObjectCleanupHandler cleanup_handler;

};

int main(int argc,char* argv[]) {

  QApplication app(argc, argv);

  QResource::registerResource("assets.rcc");
  auto settings = QWebSettings::globalSettings();
  settings->setAttribute(QWebSettings::DnsPrefetchEnabled, true);
  settings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
  settings->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
  settings->setAttribute(QWebSettings::LocalStorageEnabled, true);
  settings->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);

  QMainWindow main_window;
  auto view = new QWebView();
  auto page = new WebPage(view);
  view->setPage(page);
//  view->setFixedWidth(1024);

  auto navigate = [=](auto url) {
    qDebug() << "url: " << url;
    page->mainFrame()->evaluateJavaScript( QString("navigate('%1');").arg(url.toString()) );
  };

  auto header = new QWebView();
  header->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  QObject::connect(header->page(), &QWebPage::linkClicked, navigate);
  header->setFixedHeight(50);
  header->load(QUrl("qrc:///contents/header.html"));

  auto sidebar = new QWebView();
  sidebar->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  QObject::connect(sidebar->page(), &QWebPage::linkClicked, navigate);
  sidebar->setFixedWidth(150);
  
  sidebar->load(QUrl("qrc:///contents/sidebar.html"));


  auto header_dock = new QDockWidget();
  header_dock->setWidget(header);
  header_dock->setFeatures(
    QDockWidget::DockWidgetVerticalTitleBar
  );
  header_dock->setTitleBarWidget(new QWidget(&main_window));
  main_window.addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, header_dock);

  auto sidebar_dock = new QDockWidget();
  sidebar_dock->setWidget(sidebar);
  sidebar_dock->setTitleBarWidget(new QWidget(&main_window));

  main_window.addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, sidebar_dock);

  auto main_dock = new QDockWidget();
  main_dock->setWidget(view);
  main_dock->setTitleBarWidget(new QWidget(&main_window));

  main_window.setCentralWidget(main_dock);
  main_window.resize(800, 600);
  main_window.show();
  view->load(QUrl("qrc:///contents/main.html"));

  return app.exec();
}

#include "main.moc"
