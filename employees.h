#include <algorithm>
#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace employees {

struct Employee {
    Employee(const Employee &) = delete;
    Employee(Employee &&) = delete;
    Employee &operator=(const Employee &) = delete;
    Employee &operator=(Employee &&) = delete;

    std::string full_name;
    int base_salary_usd_per_year;

    virtual ~Employee() = default;

    explicit Employee(std::string full_name_, int base_salary_usd_per_year_);

    static std::unique_ptr<Employee> read_from(std::istream &is);

    virtual void printTo(std::ostream &) const = 0;

    [[nodiscard]] virtual int salary_usd_per_year() const = 0;
};
std::ostream &operator<<(std::ostream &os, const Employee &e);
struct Developer : virtual Employee {
    std::string github_account;

    explicit Developer(std::string full_name_,
                       int base_salary_usd_per_year_,
                       std::string github_account_);

    static std::unique_ptr<Developer> read_from(std::istream &is);

    void printTo(std::ostream &os) const override;

    [[nodiscard]] int salary_usd_per_year() const override;
};
struct Manager : virtual Employee {
    std::string project_name;

    explicit Manager(std::string full_name_,
                     int base_salary_usd_per_year_,
                     std::string project_name_);

    static std::unique_ptr<Manager> read_from(std::istream &is);

    void printTo(std::ostream &os) const override;

    [[nodiscard]] int salary_usd_per_year() const override;
};
struct LeadDeveloper final : Developer, Manager {
    explicit LeadDeveloper(std::string full_name_,
                           int base_salary_usd_per_year_,
                           std::string github_account_,
                           std::string project_name_);

    static std::unique_ptr<LeadDeveloper> read_from(std::istream &is);

    void printTo(std::ostream &os) const override;

    [[nodiscard]] int salary_usd_per_year() const override;
};
}  // namespace employees
