#include "mainwindow.h"
#include "login.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

#include "adminwindow.h"
#include "staffwindow.h"
#include "ownerwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login l;
//    int userId = -1;
//    int role = -1;


    int userId = 1;
    int role = 0;

    AdminWindow m(userId);
    m.show();
    userId = 6;
    StaffWindow s(userId);
    s.show();
    userId = 70;
    OwnerWindow o(userId);
    o.show();

    QObject::connect(&l, &Login::loginSuccess, [&](int id, int r){   //实现将数据传至其他窗口
        userId = id;
        role = r;
    });

//    if(l.exec() == QDialog::Accepted) {
//        if (role == 1) {  //物业管理员
//            AdminWindow m(userId);
//            m.show();
//            return a.exec();
//        } else if (role == 0) {  //物业工作人员
//            StaffWindow s(userId);
//            s.show();
//            return a.exec();
//        } else if (role == 2) {  //业主
//            OwnerWindow o(userId);
//            o.show();
//            return a.exec();
//        } else {
//            QMessageBox::warning(nullptr, "错误", "未知角色，无法进入系统");
//            return 0;
//        }
  //  }

    return a.exec();

}
