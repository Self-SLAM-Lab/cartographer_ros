.. Copyright 2016 The Cartographer Authors

.. Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

..      http://www.apache.org/licenses/LICENSE-2.0

.. Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

============================
Cartographer ROS Integration
============================

|build| |docs| |license|

TODO List
=======

추가적인 실제 Sensor Data를 받아서 처리되는 부분 찾기

참고 사이트
=======
* `Google Proto Buffers`_
* `Google Logging Library`_
* `Google Commandline Flags Library`_
* `Abseil CPP Library`_

* `C++ Standard Memery library`_

* `ROS CPP Intialization and Shutdown`_
* `Writing TF2 Listener`_

* `Anonymous namespace`_
* `CMake`_

.. _Google Proto Buffers: https://developers.google.com/protocol-buffers/docs/cpptutorial
.. _Google Logging Library: https://github.com/google/glog
.. _Google Commandline Flags Library: https://gflags.github.io/gflags/
.. _ROS CPP Intialization and Shutdown: http://wiki.ros.org/roscpp/Overview/Initialization%20and%20Shutdown
.. _Writing TF2 Listener: http://wiki.ros.org/tf2/Tutorials/Writing%20a%20tf2%20listener%20%28C%2B%2B%29
.. _Abseil CPP Library: https://github.com/abseil/abseil-cpp
.. _C++ Standard Memory library: https://learn.microsoft.com/ko-kr/cpp/standard-library/memory?view=msvc-170
.. _Anonymous namespace: https://jogamja.tistory.com/121
.. _CMake: https://cmake.org/cmake/help/latest/index.html

Purpose
=======

`Cartographer`_ is a system that provides real-time simultaneous localization
and mapping (`SLAM`_) in 2D and 3D across multiple platforms and sensor
configurations. This project provides Cartographer's ROS integration.

.. _Cartographer: https://github.com/cartographer-project/cartographer
.. _SLAM: https://en.wikipedia.org/wiki/Simultaneous_localization_and_mapping

Getting started
===============

* Learn to use Cartographer with ROS at `our Read the Docs site`_.
* You can ask a question by `creating an issue`_.

.. _our Read the Docs site: https://google-cartographer-ros.readthedocs.io
.. _creating an issue: https://github.com/cartographer-project/cartographer_ros/issues/new?labels=question

Contributing
============

You can find information about contributing to Cartographer's ROS integration
at `our Contribution page`_.

.. _our Contribution page: https://github.com/cartographer-project/cartographer_ros/blob/master/CONTRIBUTING.md

.. |build| image:: https://travis-ci.org/cartographer-project/cartographer_ros.svg?branch=master
    :alt: Build Status
    :scale: 100%
    :target: https://travis-ci.org/cartographer-project/cartographer_ros
.. |docs| image:: https://readthedocs.org/projects/google-cartographer-ros/badge/?version=latest
    :alt: Documentation Status
    :scale: 100%
    :target: https://google-cartographer-ros.readthedocs.io/en/latest/?badge=latest
.. |license| image:: https://img.shields.io/badge/License-Apache%202.0-blue.svg
     :alt: Apache 2 license.
     :scale: 100%
     :target: https://github.com/cartographer-project/cartographer_ros/blob/master/LICENSE

