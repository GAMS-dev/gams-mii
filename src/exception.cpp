#include "exception.h"

Exception::Exception()
{
    mStream = new QTextStream(&mBuffer);
}

Exception::Exception(const Exception &other)
    : mBuffer(other.mBuffer),
      mStream(new QTextStream(&mBuffer)),
      mInfoStart(other.mInfoStart)
{}

Exception::~Exception()
{
    qDebug() << mBuffer;
    delete mStream;
}

Exception&Exception::operator =(const Exception& other)
{
    mBuffer = other.mBuffer;
    mStream = new QTextStream(&mBuffer);
    mInfoStart = other.mInfoStart;
    return *this;
}

const char* Exception::what() const noexcept
{
    return mBuffer.isEmpty() ? QException::what()
                             : (mInfoStart <= 0 || mInfoStart >= mBuffer.length()) ? mBuffer.data()
                                                                                   : mBuffer.data()+mInfoStart;
}

QByteArray Exception::where()
{
    return (mBuffer.isEmpty() && mInfoStart > 0) ? mBuffer.left(mInfoStart-2) : QByteArray("[unknown location]");
}

FatalException::FatalException() : Exception()
{}

FatalException&FatalException::operator =(const FatalException& other)
{
    Exception::operator =(other);
    return *this;
}

FatalException* FatalException::clone() const
{
    return new FatalException(*this);
}
