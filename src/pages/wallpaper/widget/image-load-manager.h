/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#ifndef IMAGE_LOAD_MANAGER_H
#define IMAGE_LOAD_MANAGER_H

#include <QFutureWatcher>
#include <QObject>
#include <QPixmap>
#include <QQueue>
#include <QSize>

class ImageLoadManager : public QObject
{
    Q_OBJECT
private:
    ImageLoadManager(QObject *parent = nullptr);
    void init();

public:
    static ImageLoadManager *instance();
    ~ImageLoadManager();

    /* 加载指定路径图片，转换成指定大小,生成任务加入队列 */
    void load(QString imagePath, QSize size);
    /* 停止当前的加载任务并删除所有的加载任务 */
    void reset();

private slots:
    /* QFutureWatcher finished槽函数 */
    void onLoadFinished();

signals:
    /**
     * @brief 图片加载完成信号
     * @param imagePath 加载完成的图片位置
     * @param fitSize   适用于哪个大小的图片项
     * @param pixmap    加载完成的图片
     */
    void imageLoaded(QString imagePath, QSize fitSize, QPixmap pixmap);

private:
    /**
     * @brief 异步加载图片的实现函数,通过QtConcurrent::run在线程中异步执行
     * @param imagePath 图片位置
     * @param size      需要保持比例拉伸匹配什么大小
     * @return 加载和拉伸之后的图片,可通过std::move直接以避免内存拷贝
     */
    static QPixmap loadPixmap(QString imagePath, QSize size);
    QString getCacheFilePath(QString imagePath, QSize size);
    void handlerNextLoadReq();

private:
    QQueue<QPair<QString, QSize>> m_loadReqQueue;
    QFutureWatcher<QPixmap> m_loadFutureWatcher;
};

#endif  //IMAGE_LOAD_MANAGER_H
