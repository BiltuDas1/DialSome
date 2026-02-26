#ifndef SECURESTORAGE_H
#define SECURESTORAGE_H

#include <QString>
#include <QJniObject>

class SecureStorage {
public:
    explicit SecureStorage();

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
