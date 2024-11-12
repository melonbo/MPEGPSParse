#include <QThread>
#include <QFile>
#include <QByteArray>
#include <QStringList>
#include <QDebug>

class FileReaderThread : public QThread {
    Q_OBJECT

public:
    FileReaderThread(const QString &filePath, QObject *parent = nullptr)
        : QThread(parent), m_filePath(filePath) {}

signals:
    void dataReady(const QList <QByteArray> &segments);

protected:
    void run() override {
        QFile file(m_filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to open file:" << m_filePath;
            return;
        }

        QByteArray data = file.readAll();
        file.close();

        // 定义要查找的字符串
        const QByteArray delimiter = QByteArray::fromHex("000001BA");
        QList<QByteArray> segments;

        int lastIndex = 0;
        int index;

        // 查找并分割数据
        while ((index = data.indexOf(delimiter, lastIndex)) != -1) {
            QByteArray segment = data.mid(lastIndex, index - lastIndex);
            if (!segment.isEmpty()) {
                segments.append(delimiter + segment);
//                qDebug() << segment;
//                qDebug() << "size " << segment.size();
            }
            lastIndex = index + delimiter.size();
        }

        // 处理最后一个段
        if (lastIndex < data.size()) {
            QByteArray segment = data.mid(lastIndex);
            if (!segment.isEmpty()) {
                segments.append(delimiter + segment);
            }
        }
        emit dataReady(segments);
    }

private:
    QString m_filePath;
};
