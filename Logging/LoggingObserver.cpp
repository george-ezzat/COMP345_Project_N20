#include "LoggingObserver.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

//Copy constructor
Subject::Subject(const Subject& other) {
    observers = other.getObservers();
}

//Assignment operator
Subject& Subject::operator=(const Subject& other) {
    if (this != &other) {
        observers = other.getObservers();
    }
    return *this;
}
//attach observer to subject 
void Subject::attach(Observer* observer) {
    if (!observer) {
        return;
    }
    if (std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
    }
}
//detach observer from subject
void Subject::detach(Observer* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}
//notify all observers of the subject
void Subject::notifyObservers() const {
    const ILoggable* loggable = dynamic_cast<const ILoggable*>(this);
    if (loggable) {
        notifyObservers(*loggable);
    }
}
//notify all observers of the subject with a loggable object
void Subject::notifyObservers(const ILoggable& loggable) const {
    for (Observer* observer : observers) {
        if (observer) {
            observer->update(loggable);
        }
    }
}

void Subject::propagateObserversTo(Subject& target) const {
    for (Observer* observer : observers) {
        target.attach(observer);
    }
}

const std::vector<Observer*>& Subject::getObservers() const {
    return observers;
}

LogObserver::LogObserver(const std::string& filename)
    : logFilename(filename) {
    ensureLogFileExists();
}
//update the log file with the loggable object
void LogObserver::update(const ILoggable& loggable) {
    std::ofstream logFile(logFilename, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "LogObserver: Failed to open log file '" << logFilename << "'." << std::endl;
        return;
    }
    logFile << loggable.stringToLog() << std::endl;
}

std::string LogObserver::getLogFilename() const {
    return logFilename;
}

void LogObserver::setLogFilename(const std::string& filename) {
    if (filename.empty()) {
        return;
    }
    logFilename = filename;
    ensureLogFileExists();
}

void LogObserver::ensureLogFileExists() const {
    struct stat buffer;
    if (stat(logFilename.c_str(), &buffer) != 0) {
        std::ofstream createFile(logFilename);
        if (!createFile.is_open()) {
            std::cerr << "LogObserver: Unable to create log file '" << logFilename << "'." << std::endl;
        }
    }
}


