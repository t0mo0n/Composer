#ifndef MYOBJECTPOOL_H
#define MYOBJECTPOOL_H


#include <QSharedPointer>
#include <QList>

template <typename T>
class MyObjectPool
{
public:
    MyObjectPool();
    explicit MyObjectPool(int initNum); // 注意，这个初始化函数也会增加对象池的大小！
    ~MyObjectPool();

    QSharedPointer<T> acquire();
    void release(QSharedPointer<T>& object);

    int get_pool_size(){ return pool_size;} // 在文件读入里，用来设置最大大小

private:
    QList<QSharedPointer<T>> pool;
    int pool_size; // 这个是对象池的最大大小 在读入存档时，对象池只用读入这个数字就行，要结合场上已有的belt数量和最大大小判定应该初始化多大的对象池

    void expandPool(int expSize);
};

template<typename T>
MyObjectPool<T>::MyObjectPool()
{
    pool_size = 0;
}

template<typename T>
MyObjectPool<T>::MyObjectPool(int initNum)
{
    pool_size = 0;
    expandPool(initNum);
}

template<typename T>
MyObjectPool<T>::~MyObjectPool()
{
    pool.clear();
}

template<typename T>
QSharedPointer<T> MyObjectPool<T>::acquire()
{
    if (pool.isEmpty()) {
        expandPool(10);
    }

    return pool.takeFirst();
}

template<typename T>
void MyObjectPool<T>::release(QSharedPointer<T> &object)
{
    pool.append(object);
}

template<typename T>
void MyObjectPool<T>::expandPool(int expSize)
{
    for (int i = 0; i < expSize; ++i) {
        pool.append(QSharedPointer<T>(new T));
    }
    pool_size += expSize;
}

#endif // MYOBJECTPOOL_H
