#-------------------------------------------------
#
# Project created by QtCreator 2025-07-06T14:23:24
#
#-------------------------------------------------

QT       += core gui sql
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    login.cpp \
    adminwindow.cpp \
    ownerwindow.cpp \
    staffmanagewidget.cpp \
    leaveapprovewidget.cpp \
    attendancewidget.cpp \
    staffscheduling.cpp \
    training.cpp \
    databasemanager.cpp \
    houseinformationwidget.cpp \
    facilityinformationwidget.cpp \
    parkingspaceallocationwidget.cpp \
    chargingstandardsettingwidget.cpp \
    attendancecheckwidget.cpp \
    carrecordwidget.cpp \
    carrenterwidget.cpp \
    houserenterwidget.cpp \
    paymentinquirywidget.cpp \
    repairworkorderwidget.cpp \
    rosterquerywidget.cpp \
    staffleaveswidget.cpp \
    staffwindow.cpp \
    visitorregistrationwidget.cpp \
    complaintrecordpage.cpp \
    expendituredetailpage.cpp \
    feereportpage.cpp \
    incomedetailpage.cpp \
    summarypage.cpp \
    announcementswidget.cpp \
    dailyexpensedetailpage.cpp \
    dailyincomedetailpage.cpp \
    monthlyexpensedetailpage.cpp \
    monthlyincomedetailpage.cpp \
    yearlyexpensedetailpage.cpp \
    yearlyincomedetailpage.cpp \
    bill.cpp \
    carregistration.cpp \
    complaints.cpp \
    evaluation.cpp \
    history.cpp \
    MyHome.cpp \
    parking.cpp \
    maintenance.cpp \
    announcementwindow.cpp \
    databasehelper.cpp \
    initiativewidget.cpp \
    noticewidget.cpp \
    publicitywidget.cpp \
    warningwidget.cpp

HEADERS += \
        mainwindow.h \
    login.h \
    adminwindow.h \
    ownerwindow.h \
    staffmanagewidget.h \
    leaveapprovewidget.h \
    attendancewidget.h \
    staffscheduling.h \
    training.h \
    databasemanager.h \
    houseinformationwidget.h \
    facilityinformationwidget.h \
    parkingspaceallocationwidget.h \
    chargingstandardsettingwidget.h \
    attendancecheckwidget.h \
    carrecordwidget.h \
    carrenterwidget.h \
    houserenterwidget.h \
    paymentinquirywidget.h \
    repairworkorderwidget.h \
    rosterquerywidget.h \
    staffleaveswidget.h \
    staffwindow.h \
    visitorregistrationwidget.h \
    complaintrecordpage.h \
    expendituredetailpage.h \
    feereportpage.h \
    incomedetailpage.h \
    summarypage.h \
    announcementswidget.h \
    dailyexpensedetailpage.h \
    dailyincomedetailpage.h \
    monthlyexpensedetailpage.h \
    monthlyincomedetailpage.h \
    yearlyexpensedetailpage.h \
    yearlyincomedetailpage.h \
    bill.h \
    carregistration.h \
    complaints.h \
    evaluation.h \
    history.h \
    MyHome.h \
    parking.h \
    maintenance.h \
    announcementwindow.h \
    databasehelper.h \
    initiativewidget.h \
    noticewidget.h \
    publicitywidget.h \
    warningwidget.h

FORMS += \
        mainwindow.ui \
    login.ui \
    adminwindow.ui \
    ownerwindow.ui \
    staffmanagewidget.ui \
    leaveapprovewidget.ui \
    attendancewidget.ui \
    staffscheduling.ui \
    training.ui \
    houseinformationwidget.ui \
    facilityinformationwidget.ui \
    parkingspaceallocationwidget.ui \
    chargingstandardsettingwidget.ui \
    attendancecheckwidget.ui \
    carrecordwidget.ui \
    carrenterwidget.ui \
    houserenterwidget.ui \
    paymentinquirywidget.ui \
    repairworkorderwidget.ui \
    rosterquerywidget.ui \
    staffleaveswidget.ui \
    staffwindow.ui \
    visitorregistrationwidget.ui \
    complaintrecordpage.ui \
    expendituredetailpage.ui \
    feereportpage.ui \
    incomedetailpage.ui \
    summarypage.ui \
    announcementswidget.ui \
    dailyexpensedetailpage.ui \
    dailyincomedetailpage.ui \
    monthlyexpensedetailpage.ui \
    monthlyincomedetailpage.ui \
    yearlyexpensedetailpage.ui \
    yearlyincomedetailpage.ui \
    bill.ui \
    carregistration.ui \
    complaints.ui \
    evaluation.ui \
    history.ui \
    MyHome.ui \
    parking.ui \
    maintenance.ui \
    announcementwindow.ui \
    initiativewidget.ui \
    noticewidget.ui \
    publicitywidget.ui \
    warningwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc

DISTFILES += \
    parkingspace.jpg
