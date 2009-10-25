<?php
ob_start();
/*
**     Ths program is free software: you can redistribute it and/or modify
**     it under the terms of the GNU General Public License as published by
**     the Free Software Foundation, either version 3 of the License, or
**     (at your option) any later version.
**
**     This program is distributed in the hope that it will be useful,
**     but WITHOUT ANY WARRANTY; without even the implied warranty of
**     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**     GNU General Public License for more details.
**
**     You should have received a copy of the GNU General Public License
**     along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
?>

<?php
/*      This code is based on the nag-lite project found on http://nagiosexchange.org
**
**      this code is tested on nagios version 2 and 3
**
**      Bo Larsen, ha9bal@gmail.com
*/
?>

<?php
     header("Pragma: no-cache");
     header("Refresh: 90");
?>

<HTML>
<HEAD>
<TITLE>Nagios Status Lite</TITLE>
</HEAD>
<BODY BGCOLOR="#FFFFFF">

<CENTER> <H2>Nagios System Status</H2></CENTER>
<CENTER>

<?php
    	require './naglite.inc';

    	$status_lines = read_status_file($status_file);

    	parse_status_array($status_lines);

#	echo "nagios version : " . $pstatus["version"]."<br /><br />";
	
	if(is_array($hlist)) 
	foreach (array_keys($hlist) as $hkey) {
		if( $hlist[$hkey]["host"]["status"] != "UP" && $hlist[$hkey]["host"]["status"] != "PENDING") {
			if($hlist[$hkey]["host"]["status"] == "UNREACHABLE") { $unreachable_hosts++; } 
			else if($hlist[$hkey]["host"]["status"] == "PENDING") { $pending_hosts++; } 
			else { $badhosts++;}
		} else {
			$goodhosts++;
		}
	}

	echo "<TABLE BORDER=0 celspacing=2 width=80%>\n";
	echo "<TR>\n<TH><FONT SIZE =+4>HOST Status</FONT></TH></TR>";
	if($goodhosts)  { echo "<TR>\n<TD bgcolor=\"green\" align=center><FONT COLOR=\"white\" SIZE=+4>$goodhosts UP</FONT></TD>\n</TR>"; }
	if($unreachable_hosts)  { echo "<TR>\n<TD bgcolor=\"orange\" align=center><FONT COLOR=\"white\" SIZE=+4>$unreachable_hosts Unreachable</FONT></TD>\n</TR>"; }
	if($pending_hosts)  { echo "<TR>\n<TD bgcolor=\"grey\" align=center><FONT COLOR=\"white\" SIZE=+4>$pending_hosts Pending</FONT></TD>\n</TR>"; }
	if($badhosts)  { echo "<TR>\n<TD bgcolor=\"red\" align=center><FONT COLOR=\"white\" SIZE=+4>$badhosts DOWN</FONT></TD>\n</TR>"; }
	echo "</TABLE>\n";

	echo "<P>\n";

	echo "<TABLE BORDER=0 celspacing=2 width=80%>\n";
	echo "<TR>\n<TH><FONT SIZE =+4>Service Status</FONT></FONT></TH></TR>";

	if($services_ok) { echo "<TR>\n<TD bgcolor=\"green\" align=center><FONT COLOR=\"white\" SIZE=+4>$services_ok OK</FONT></TD>\n</TR>"; }
	if($services_warning) { echo "<TR>\n<TD bgcolor=\"orange\" align=center><FONT COLOR=\"white\" SIZE=+4>$services_warning Warning</FONT></TD>\n</TR>"; }
	if($services_critical) { echo "<TR>\n<TD bgcolor=\"red\" align=center><FONT COLOR=\"white\" SIZE=+4>$services_critical Critical</FONT></TD>\n</TR>"; }
	if($services_pending) { echo "<TR>\n<TD bgcolor=\"grey\" align=center><FONT COLOR=\"white\" SIZE=+4>$services_pending Pending</FONT></TD>\n</TR>"; }
	if($service_unknown) { echo "<TR>\n<TD bgcolor=\"yellow\" align=center><FONT COLOR=\"white\" SIZE=+4>$services_unknown Unknown</FONT></TD>\n</TR>"; }
        echo "\n</TABLE>\n";
/*
	echo "<P>";

	echo "<TABLE BORDER=0 celspacing=2 width=80%>\n";
	echo "<TR>\n<TH><FONT SIZE =+1>Nagios Monitor Status</FONT></TH>\n</TR>\n";
	echo "<TR>\n";
	echo "<TD>Process ID: </TD>\n". "<TD>".$pstatus["nagios_pid"]."</TD>\n";
	echo "</TR>\n";
	echo "<TR>\n";
	echo "<TD>Check Status: </TD>\n". "<TD>".$pstatus["nagios_status"]."</TD>\n";
	echo "</TR>\n";
	echo "<TR>\n";
	echo "<TD>Webpage refresh interval (s): </TD>\n". "<TD>90</TD>\n";
	echo "</TR>\n";
	echo "</TABLE>\n";
*/
?>

</CENTER>
</BODY>
</HTML>
