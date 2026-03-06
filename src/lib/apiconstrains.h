#include <QString>


namespace API {
  const QString sourceRepo = GITHUB_REPO;

  namespace Voice {
    const QString call = "/voicecall/send";
  }

  namespace Auth {
    const QString loginUser = "/users/login";
    const QString registerUser = "/users/register";
    const QString refresh = "/token/refresh";
  }

  namespace FCM {
    const QString updateDevice = "/fcm/update";
  }
}
