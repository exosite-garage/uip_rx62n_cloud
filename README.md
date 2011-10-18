========================================
About uip_rx62n_cloud
========================================
This project is an example HEW project demonstrating how to use the Renesas
RX62N RDK to send and receive data to/from the cloud by using Exosite's Cloud 
Data Platform.  The project is based on the Renesas toolchain's UIP networking 
stack example application.  The communication with the cloud is accomplished 
over HTTP in a RESTful style interface.  The project functionality writes the 
value of the RDK's on-board potentiometer to the cloud and reads a data source 
from the cloud and displays both values on the on-board LCD screen.  This 
project can be re-used, extended and deployed as desired.

License is BSD, Copyright 2011, Exosite LLC (see LICENSE file)

Tested with HEW version 4.09.00.007, including:<br>
C/C++ compiler package for RX family V.1.01 Release 00	(6-14-2011 21:50:37)<br>
Renesas Demonstration Kit (RDK) RX62N v3.00 Release 00	(6-15-2011 00:08:55)<br>

========================================
Quick Start
========================================
1) Install HEW, the RX toolchain and the RDK demo projects<br>
* HINT: packages can be downloaded from Renesas at http://tinyurl.com/42turtk<br>

2) Open the uip_rx62n_cloud.hws project workspace with HEW<br>
* NOTE: The file "user_app.c" is the primary user app - modify to suit<br>

3) Edit the "RECOVERYCIK" value in user_app.c to match your CIK value<br>
* HINT: Obtain a CIK from https://renesas.exosite.com by clicking +Add Device<br>
* NOTE: Your RDK should already have a CIK value associated, so this is just
        a disaster recovery setp and is not requried<br>

4) Compile and download -> check https://renesas.exosite.com to see your data
in the cloud!<br>
* HINT: Your RDK must be connected to the Internet via the RJ-45 ethernet jack<br>
* HINT: If your network does not support DHCP, you will need to edit the network
values in user_app.c (my_ip, my_netmask, my_default_router)<br>

For more information on this project and other examples, reference the online 
documentation at http://exosite.com/renesas<br>

========================================
Release Info
========================================
----------------------------------------
Release 2011-10-18
----------------------------------------
--) renamed project from rx62n_rdk_cloud to uip_rx62n_cloud<br>

----------------------------------------
Release 2011-09-05
----------------------------------------
--) synchronized with Micrium source<br>

----------------------------------------
Release 2011-08-31
----------------------------------------
--) moved away from PSOCK interface in favor of straight uip calls<br>

----------------------------------------
Release 2011-08-26
----------------------------------------
--) initial version<br>
