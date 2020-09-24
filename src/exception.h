#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QException>
#include <QTextStream>
#include <QDebug>

class Exception : public QException
{
public:
    Exception();
    Exception(const Exception &other);
    ~Exception();
    Exception &operator =(const Exception& other);

    void raise() const { throw *this; }
    Exception *clone() const { return new Exception(*this); }

    Exception& operator<<(const char value) {
        if (mInfoStart < 0) {
            (*mStream) << '[';
            mInfoStart++;
        }
        if (mInfoStart == 0 && value=='\t') {
            (*mStream) << "] ";
            (*mStream).flush();
            mInfoStart = mBuffer.length();
        } else {
            (*mStream) << value;
            (*mStream).flush();
        }
        return *this;
    }

    template <typename T> Exception& operator<<(const T& value) {
        if (mInfoStart < 0) {
            (*mStream) << '[';
            mInfoStart = 0;
        }
        (*mStream) << value;
        (*mStream).flush();
        return *this;
    }
    const char* what() const noexcept;
    virtual QByteArray where();

protected:
    QByteArray mBuffer;
    QTextStream *mStream;
    int mInfoStart = -1;
};

class FatalException : public Exception
{
public:
    FatalException();
    FatalException &operator =(const FatalException& other);
    template <typename T> FatalException& operator<<(const T& value) {
        Exception::operator <<(value);
        return *this;
    }
    FatalException(const FatalException &exception) : Exception(exception) {}
    void raise() const { throw *this; }
    FatalException *clone() const;
};

#ifdef QT_DEBUG
#  ifdef __GNUC__
#    define EXCEPT() throw Exception() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__ << '\t'
#    define FATAL() throw FatalException() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__ << '\t'
#  else
#    define EXCEPT() throw Exception() << __FUNCSIG__ << __FILE__ << __LINE__ << '\t'
#    define FATAL() throw FatalException() <<__FUNCSIG__ << __FILE__ << __LINE__ << '\t'
#  endif
#else
#  define EXCEPT() throw Exception()
#  define FATAL() throw FatalException()
#endif

#endif // EXCEPTION_H
