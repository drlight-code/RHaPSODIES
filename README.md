# RHaPSODIES

Real-Time Hand-Pose Particle Swarm Optimization for Dual-Handed
Interaction with Electronic Systems. A dual-hand tracker using RGB-D
cameras.


## About

This project implements [1]. The aim is to create:

* general purpose RGB-D hand tracking framework as independent library
* device drivers for the ViSTA Virtual Reality Toolkit
* a demo application

Let's see how for we can get...


## License

The work is licensed under the GNU General Public License Version
3, see LICENSE.

**PLEASE NOTE:** As of now, the SkinClassifierRedMatter* classes use
code which is licensed under CPOL (Code Project Open License,
http://www.codeproject.com/info/cpol10.aspx) which is *not* compatible
with the GPL. Before this project can be deployed in application or
library form, we need to factor out the RedMatter code in it's own
library which will then be dynamically linked!


## Contact

patric.schmitz@rwth-aachen.de


## References

[1] Tracking the Articulated Motion of Two Strongly Interacting Hands,
I. Oikonomidis, N. Kyriazis, A.A. Argyros, IEEE CVPR 2012
