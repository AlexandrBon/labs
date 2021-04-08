#include "employees.h"
#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace employees {

Employee::Employee(std::string full_name_, int base_salary_usd_per_year_)
    : full_name(std::move(full_name_)),
      base_salary_usd_per_year(base_salary_usd_per_year_) {
}

std::ostream &operator<<(std::ostream &os, const Employee &e) {
    e.printTo(os);
    return os;
}

std::unique_ptr<Developer> Developer::read_from(std::istream &is) {
    std::string full_name;
    int base_salary_usd_per_year = 0;
    std::string github_account;
    is >> full_name >> base_salary_usd_per_year >> github_account;
    std::replace(full_name.begin(), full_name.end(), '~', ' ');
    return std::make_unique<Developer>(std::move(full_name),
                                       base_salary_usd_per_year,
                                       std::move(github_account));
}

void Developer::printTo(std::ostream &os) const {
    std::string tmp_name = full_name;
    std::replace(tmp_name.begin(), tmp_name.end(), ' ', '~');
    os << "Developer " << tmp_name << " " << base_salary_usd_per_year << " "
       << github_account;
}

int Developer::salary_usd_per_year() const {
    return static_cast<int>(github_account.size()) * 100 +
           base_salary_usd_per_year;
}

Manager::Manager(std::string full_name_,
                 int base_salary_usd_per_year_,
                 std::string project_name_)
    : Employee(std::move(full_name_), base_salary_usd_per_year_),
      project_name(std::move(project_name_)) {
}

void Manager::printTo(std::ostream &os) const {
    std::string tmp_name = full_name;
    std::replace(tmp_name.begin(), tmp_name.end(), ' ', '~');
    os << "Manager " << tmp_name << " " << base_salary_usd_per_year << " "
       << project_name;
}

std::unique_ptr<Manager> Manager::read_from(std::istream &is) {
    std::string full_name;
    int base_salary_usd_per_year = 0;
    std::string project_name;
    is >> full_name >> base_salary_usd_per_year >> project_name;
    std::replace(full_name.begin(), full_name.end(), '~', ' ');
    return std::make_unique<Manager>(std::move(full_name),
                                     base_salary_usd_per_year,
                                     std::move(project_name));
}

int Manager::salary_usd_per_year() const {
    return static_cast<int>(project_name.size()) * 100 +
           base_salary_usd_per_year;
}

Developer::Developer(std::string full_name_,
                     int base_salary_usd_per_year_,
                     std::string github_account_)
    : Employee(std::move(full_name_), base_salary_usd_per_year_),
      github_account(std::move(github_account_)) {
}

LeadDeveloper::LeadDeveloper(std::string full_name_,
                             int base_salary_usd_per_year_,
                             std::string github_account_,
                             std::string project_name_)
    : Employee(std::move(full_name_), base_salary_usd_per_year_),
      Developer(std::move(full_name_),
                base_salary_usd_per_year_,
                std::move(github_account_)),
      Manager(std::move(full_name_),
              base_salary_usd_per_year_,
              std::move(project_name_)) {
}

std::unique_ptr<LeadDeveloper> LeadDeveloper::read_from(std::istream &is) {
    std::string full_name;
    int base_salary_usd_per_year = 0;
    std::string github_account;
    std::string project_name;
    is >> full_name >> base_salary_usd_per_year >> github_account >>
        project_name;
    std::replace(full_name.begin(), full_name.end(), '~', ' ');
    return std::make_unique<LeadDeveloper>(
        std::move(full_name), base_salary_usd_per_year,
        std::move(github_account), std::move(project_name));
}

void LeadDeveloper::printTo(std::ostream &os) const {
    std::string tmp_name = full_name;
    std::replace(tmp_name.begin(), tmp_name.end(), ' ', '~');
    os << "LeadDeveloper " << tmp_name << " " << base_salary_usd_per_year << " "
       << github_account << " " << project_name;
}

int LeadDeveloper::salary_usd_per_year() const {
    return static_cast<int>(project_name.size()) * 80 +
           static_cast<int>(github_account.size()) * 150 +
           base_salary_usd_per_year;
}

std::unique_ptr<Employee> Employee::read_from(std::istream &is) {
    std::string type;
    is >> type;
    if (type == "Developer") {
        return Developer::read_from(is);
    } else if (type == "Manager") {
        return Manager::read_from(is);
    } else if (type == "LeadDeveloper") {
        return LeadDeveloper::read_from(is);
    } else {
        return nullptr;
    }
}
}  // namespace employees
