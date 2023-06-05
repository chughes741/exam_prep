#ifndef WARLOCK_HPP_
#define WARLOCK_HPP_

#include <string>
#include <iostream>

class Warlock {
 public:
  Warlock(std::string name, std::string title);

 private:
  Warlock(const Warlock& other) {}
  const Warlock& operator=(const Warlock& rhs) {}

 public:
  ~Warlock();

  const std::string &getName() const;
  const std::string &getTitle() const;

  void setTitle(const std::string &title);

  void introduce() const;

 private:
  std::string name;
  std::string title;
};

#endif  // WARLOCK_HPP_
