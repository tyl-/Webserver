# Webserver [![Build Status](https://travis-ci.org/tyl-/Webserver.svg?branch=master)](https://travis-ci.org/tyl-/Webserver) [![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

This is an attempt to port an old java multithreaded webserver from my distributed systems course into a C++ version.

---

## Main Goals

The main goals for this program are:
- In Progress: **Convert Webserver**
- Scrapped: ~~~Convert Telnet~~~
- Scrapped: ~~~Convert Listener~~~

## Start Date

- March 17, 2017

## Initial Completion Date

- In Progress

## Goal Changes

- Some files are not getting converted because I realized those files were not part of the webserver sourcecode.
- I was having problems handling threads with sockets, I changed it to fork for now to allows for concurrent connections.

---

## Updates

- Last updated March 19, 2017

---

## Possible Improvements

- Use fork instead of threads.
- Find a way to incorporate threads.

---

## Notes

---

## Special Thanks To

- <a href="http://fvcproductions.com" target="_blank">FVCproductions</a> for their README Template.
- <a href="https://travis-ci.org/" target="_blank">Travis CI</a>.

---

## License

[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

- **[MIT license](http://opensource.org/licenses/mit-license.php)**