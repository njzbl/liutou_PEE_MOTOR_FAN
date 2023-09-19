#include "opertionexcel.h"


QSemaphore mQSemaphore(1);

OpertionExcel::OpertionExcel(QThread *parent) : QThread(parent)
{
    mQSemaphore.acquire(1);
}

QString path1 = "D:/test.xlsx";
void OpertionExcel::run(void)
{
//    while(1) {
//        qDebug() << "in run mQSemaphore.acquire()";
//        mQSemaphore.acquire();  //等待资源
//        qDebug() << "is mQSemaphore.acquire()";
//        //        QList<QVariant> fileName1;
//                excel = new QAxObject("Excel.Application");
//        //        QAxObject excel("Excel.Application");
//                excel->setProperty("Visible",false);
//                QAxObject *work_books = excel->querySubObject("WorkBooks");    //定义工作簿
//                work_books->dynamicCall("Open (const QString&)",QDir::toNativeSeparators(path1));     //读取选取路径文件
//                QVariant title_value = excel->property("Caption");             //获取标题(文件名)
//                qDebug()<<QString("excel title : ")<<title_value.toString();             //输出标题(文件名)
//                QAxObject *work_book = excel->querySubObject("ActiveWorkBook");//当前工作簿
//                QAxObject *work_sheets = work_book->querySubObject("Sheets");  //Sheets也可换用WorkSheets

//                int sheet_count = work_sheets->property("Count").toInt();  //获取工作表数目
//                qDebug()<<QString("sheet count : ")<<sheet_count;

//                for(int i=1; i<=sheet_count; i++)
//                {
//                QAxObject *work_sheet = work_book->querySubObject("Sheets(int)", i);  //Sheets(int)也可换用Worksheets(int) ，选择摸一个工作表
//                QString work_sheet_name = work_sheet->property("Name").toString();  //获取工作表名称
//                QString message = QString("sheet ")+QString::number(i, 10)+ QString(" name");
//                qDebug()<<message<<work_sheet_name;
//                }
//                if(sheet_count > 0)
//                {
//                QAxObject *work_sheet = work_book->querySubObject("Sheets(int)", 1);
//                QAxObject *used_range = work_sheet->querySubObject("UsedRange");
//                QAxObject *rows = used_range->querySubObject("Rows");
//                QAxObject *columns = used_range->querySubObject("Columns");
//                int row_start = used_range->property("Row").toInt();  //获取起始行
//                int column_start = used_range->property("Column").toInt();  //获取起始列
//                int row_count = rows->property("Count").toInt();  //获取行数
//                int column_count = columns->property("Count").toInt();  //获取列数
//                int a=column_start+(row_start-1)*column_count;
//                for(int i=row_start; i<=row_count; i++)
//                {
//                 for(int j=column_start; j<=column_count; j++)
//                  {
//                   QAxObject *cell = work_sheet->querySubObject("Cells(int,int)", i, j);
//                   QVariant cell_value = cell->property("Value");  //获取单元格内容  如果为空，则返回invalid

//            //重要部分，批量操作控件
//        //          QLineEdit* lE=findChild<QLineEdit*>("lineEdit_"+QString::number(a));
//        //          if (lE!=nullptr&&cell_value.isValid())
//        //          {
//        //            lE->setText(cell_value.toString());
//        //            a++;
//        //          }
//                  QString message = QString("row")+QString::number(i, 10)+QString("--column")+QString::number(j, 10)+QString(":");
//                  qDebug()<<message<<cell_value.isValid()<<cell_value.toString();
//                 }
//                }
//               }

//                work_books->dynamicCall("Close()");//关闭工作簿
//                excel->dynamicCall("Quit()");//关闭excel

//    }
}
