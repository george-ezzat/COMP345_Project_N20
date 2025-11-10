#pragma once
#ifndef LOGGING_OBSERVER_H
#define LOGGING_OBSERVER_H

#include <string>
#include <vector>

class ILoggable {
public:
    virtual ~ILoggable() = default;
    virtual std::string stringToLog() const = 0;
};

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const ILoggable& loggable) = 0;
};

class Subject {
public:
    Subject() = default;
    Subject(const Subject& other);
    Subject& operator=(const Subject& other);
    virtual ~Subject() = default;

    void attach(Observer* observer);
    void detach(Observer* observer);

protected:
    void notifyObservers() const;
    void notifyObservers(const ILoggable& loggable) const;
    void propagateObserversTo(Subject& target) const;
    const std::vector<Observer*>& getObservers() const;

private:
    mutable std::vector<Observer*> observers;
};

class LogObserver : public Observer {
public:
    explicit LogObserver(const std::string& filename = "gamelog.txt");
    ~LogObserver() override = default;

    void update(const ILoggable& loggable) override;
    std::string getLogFilename() const;
    void setLogFilename(const std::string& filename);

private:
    std::string logFilename;
    void ensureLogFileExists() const;
};

#endif 

