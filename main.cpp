#include <QApplication>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QWidget>
#include <QWindow>
#include <cstdio>
#include <iostream>
#include <vector>

class PinImageViewer : public QWidget {
   public:
    PinImageViewer(const QImage& image) {
        // Set frameless + stay on top + tool window
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);

        setAttribute(Qt::WA_TranslucentBackground);

        originalPixmap = QPixmap::fromImage(image);

        label = new QLabel(this);
        label->setScaledContents(true);
        label->setMinimumSize(1, 1);
        label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(label);

        resize(originalPixmap.size());
        label->setPixmap(originalPixmap);
    }

    void prepareWaylandWindow() {
        create();  // Create native window handle

        QWindow* win = windowHandle();

        if (win) {
            win->setProperty("_q_wayland_layer_shell_layer", "overlay");
            win->setProperty("_q_wayland_layer_shell_keyboard_interactivity", 0);
        }
    }

   protected:
    void closeEvent(QCloseEvent* event) override {
        QWidget::closeEvent(event);

        QCoreApplication::quit();  // Force terminate app.exec()
    }

    // Double left-click -> Restore original size
    void mouseDoubleClickEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            resize(originalPixmap.size());
        } else {
            QWidget::mouseDoubleClickEvent(event);
        }
    }

    // Left mouse button drag to move window
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            if (this->windowHandle()) {
                this->windowHandle()->startSystemMove();
            }
        }
    }

    // Mouse wheel zoom
    void wheelEvent(QWheelEvent* event) override {
        float factor = (event->angleDelta().y() > 0) ? 1.1f : 0.9f;

        QSize newSize = size() * factor;

        if (newSize.width() >= 10 && newSize.height() >= 10) {
            resize(newSize);
        }
    }

    // Press Esc or Q to trigger close()
    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Q) {
            this->close();
        }
    }

   private:
    QLabel* label;

    QPixmap originalPixmap;
};

int main(int argc, char* argv[]) {
    // Completely disable Wayland input method module to prevent zwp_text_input deadlock in GNOME
    qputenv("QT_IM_MODULE", "none");

    std::ios_base::sync_with_stdio(false);

    QApplication app(argc, argv);

    // Read data from standard input
    std::vector<uchar> buffer;

    char chunk[4096];

    while (std::cin.read(chunk, sizeof(chunk))) {
        buffer.insert(buffer.end(), chunk, chunk + std::cin.gcount());
    }

    if (std::cin.gcount() > 0) {
        buffer.insert(buffer.end(), chunk, chunk + std::cin.gcount());
    }

    // Close stdin immediately after reading to unlock pipeline/terminal
    fclose(stdin);

    if (buffer.empty()) {
        std::cerr << "Error: No image data read from standard input!" << std::endl;

        return -1;
    }

    QImage image;

    if (!image.loadFromData(buffer.data(), static_cast<int>(buffer.size()))) {
        std::cerr << "Error: Failed to parse image data!" << std::endl;

        return -1;
    }

    PinImageViewer viewer(image);

    viewer.prepareWaylandWindow();

    viewer.show();

    return app.exec();
}
