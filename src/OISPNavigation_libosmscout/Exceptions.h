#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

class FileNotFoundException : public std::exception
{
public:
  FileNotFoundException(const std:: string &file) : txt(file) {}

  const char *what() const throw();

private:
  const std::string &txt;
};

#endif // EXCEPTIONS_H
