#ifndef SECURESTORAGE_H
#define SECURESTORAGE_H

#include <QString>
#include <QJniObject>
#include <QObject>

class SecureStorage : public QObject {
    Q_OBJECT
    
public:
    explicit SecureStorage(QObject *parent = nullptr);

    void save(const QString &key, const QString &value);
    QString get(const QString &key);
    void clear();

    void saveRefreshToken(const QString &token);
    QString getRefreshToken();
    bool exists(const QString &key);

private:
    QJniObject m_javaObject;
    bool isValid() const;
};

#endif // SECURESTORAGE_H
