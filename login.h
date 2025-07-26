#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "databasemanager.h"
#include <QLineEdit>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QPushButton* loginBtn;
    QPushButton* registerBtn;


signals:
    void loginSuccess(int userID,int role);

private slots:
    void onLoginClicked();
    void onExitClicked();

private:
    Ui::Login *ui;
    DatabaseManager& db;
};

#endif // LOGIN_H
