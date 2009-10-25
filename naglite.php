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

/*      This code is based on the nag-lite project found on http://nagiosexchange.org
**
**      this code is tested on nagios version 2 and 3
**
**      Bo Larsen, ha9bal@gmail.com
*/

header("Pragma: no-cache");
header("Refresh: 90");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
  "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
  <head>
    <meta http-equiv="Content-type" content="text/html; charset=utf-8">
    <title>Nagios Status Lite</title>	
    <!-- Framework CSS -->
    <link rel="stylesheet" href="./css/screen.css" type="text/css" media="screen, projection">
    <link rel="stylesheet" href="./css/print.css" type="text/css" media="print">

    <!--[if lt IE 8]><link rel="stylesheet" href="./css/ie.css" type="text/css" media="screen, projection"><![endif]-->

    <!-- Import fancy-type plugin for the sample page. -->
    <link rel="stylesheet" href="./css/plugins/fancy-type/screen.css" type="text/css" media="screen, projection">
  </head>
  <body>

	<CENTER> <H2>Nagios System Status</H2></CENTER>
	<CENTER>

<?php
        require_once './naglite.inc';


    	$status_lines = read_status_file($status_file);

    	parse_status_array($status_lines);

	echo "nagios version : " . $pstatus["version"]."<br /><br />";

	if(is_array($hlist)) 
	foreach (array_keys($hlist) as $hkey) {
		if( $hlist[$hkey]["host"]["status"] != "UP" && $hlist[$hkey]["host"]["status"] != "PENDING") {
			$hostsout .= sprintf("<TR bgcolor=\"pink\">\n");
			$hostsout .= sprintf("\t<TD bgcolor=\"lightgrey\">%s</TD> ", $hlist[$hkey]["host"]["host_name"]);
			$hostsout .= sprintf("<TD>%s</TD>", $hlist[$hkey]["host"]["notifications_enabled"] ? "ON" : "OFF");
			if($hlist[$hkey]["host"]["status"] == "UNREACHABLE") {
				$hostsout .= sprintf("<TD BGCOLOR=\"orange\">%s</TD>", $hlist[$hkey]["host"]["status"]);
			} else {
				$hostsout .= sprintf("<TD BGCOLOR=\"red\">%s</TD>", $hlist[$hkey]["host"]["status"]);
			}
			$hostsout .= sprintf("<TD>%s</TD>", $hlist[$hkey]["host"]["last_check"]);
			$hostsout .= sprintf("<TD>%s</TD>", $hlist[$hkey]["host"]["last_state_change"]);
			# $hostsout .= sprintf("<TD>%s</TD>", $hlist[$hkey]["host"]["last_notification"]);
			$hostsout .= sprintf("<TD>%s</TD>\n", $hlist[$hkey]["host"]["plugin_output"]);
			$hostsout .= sprintf("</TR>\n");
			if($hlist[$hkey]["host"]["status"] == "UNREACHABLE") { $unreachable_hosts++; } 
			else if($hlist[$hkey]["host"]["status"] == "PENDING") { $pending_hosts++; } 
			else { $badhosts++;}
		} else {
			$goodhosts++;
		}

		if(! $hlist[$hkey]["host"]["notifications_enabled"] ) {
			$notifications_off .= $hlist[$hkey]["host"]["host_name"]. "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ";
		}

		$services=$hlist[$hkey]["service"];

		$lastalert = "none";

		if(is_array($services) && $hlist[$hkey]["host"]["status"] == "UP") {

			foreach( $services as $service => $svalue) {
				if($services[$service]["status"] == "WARNING") {
					$servicesout_warning .= "<TR bgcolor=\"orange\">\n";

					if($hlist[$hkey]["host"]["host_name"] == $lasthost && $lastalert == "WARNING" || $lastalert == "CRITICAL")  {
						$servicesout_warning .= sprintf("<TD bgcolor=\"white\">&nbsp;</TD>");
					} else {
						$servicesout_warning .= sprintf("<TD bgcolor=\"lightgrey\">%s</TD>", 
                                                                                   $hlist[$hkey]["host"]["host_name"]);
					}

					$servicesout_warning .= sprintf("<TD>%s</TD>", $services[$service]["description"]);
					$servicesout_warning .= sprintf("<TD BGCOLOR=\"yellow\">%s</TD>", $services[$service]["status"]);
					# $servicesout_warning .= sprintf("<TD>%s</TD>", $services[$service]["last_check"]);
					$servicesout_warning .= sprintf("<TD>%s</TD>", $services[$service]["last_state_change"]);
					$servicesout_warning .= sprintf("<TD>%s</TD>", $services[$service]["duration"]);
					# $servicesout_warning .= sprintf("<TD>%s/%s</TD>", $services[$service]["current_attempt"], $services[$service]["max_attempts"]);
					$servicesout_warning .= sprintf("<TD>%s</TD>", $services[$service]["plugin_output"]);
					$servicesout_warning .= sprintf("</TR>\n");
					$lastalert="warning";
				} else {
					$servicesout_error .= "<TR bgcolor=\"pink\">\n";
					if($hlist[$hkey]["host"]["host_name"] != $lasthost && $hlist[$hkey] == "CRITICAL" || $lasterror != "WARNING")  {
						$servicesout_error .= sprintf("<TD bgcolor=\"lightgrey\">%s</TD>", $hlist[$hkey]["host"]["host_name"]);
					} else {
						$servicesout_error .= sprintf("<TD bgcolor=\"white\">&nbsp;</TD>");
					}
					$servicesout_error .= sprintf("<TD>%s</TD>", $services[$service]["description"]);
					$servicesout_error .= sprintf("<TD BGCOLOR=\"red\">%s</TD>", $services[$service]["status"]);
					# $servicesout_error .= sprintf("<TD>%s</TD>", $services[$service]["last_check"]);
					$servicesout_error .= sprintf("<TD>%s</TD>", $services[$service]["last_state_change"]);
					$servicesout_error .= sprintf("<TD>%s</TD>", $services[$service]["duration"]);
					# $servicesout_error .= sprintf("<TD>%s/%s</TD>", $services[$service]["current_attempt"], $services[$service]["max_attempts"]);
					$servicesout_error .= sprintf("<TD>%s</TD>", $services[$service]["plugin_output"]);
					$servicesout_error .= sprintf("</TR>\n");
					$lastalert="error";
				}
				$lastalert="none";

				$lasthost = $hlist[$hkey]["host"]["host_name"];
			}
		}
	}

	echo "<TABLE BORDER=0 celspacing=2 width=80%>\n";
	echo "<TR>\n<TH COLSPAN=3>&nbsp</TH><TH COLSPAN=2><FONT SIZE =+1>HOST Status</FONT></TH><TH>";
	if($goodhosts)  { echo "<FONT COLOR=\"green\">$goodhosts UP</FONT> &nbsp; "; }
	if($unreachable_hosts)  { echo "<FONT COLOR=\"orange\">$unreachable_hosts Unreachable</FONT> &nbsp; "; }
	if($pending_hosts)  { echo "<FONT COLOR=\"grey\">$pending_hosts Pending</FONT> &nbsp; "; }
	if($badhosts)  { echo "<FONT COLOR=\"red\">$badhosts DOWN</FONT>"; }
	echo "</TH>\n</TR>\n";
	if(strlen($hostsout)) {
		echo "<TR bgcolor=\"lightgrey\">\n";
		echo "\t<TH>Host</TH><TH>Notifications</TH><TH>Status</TH><TH>Last Check</TH><TH>Last Change</TH><TH>Status Information</TH>\n";
		echo "</TR>\n";
		echo $hostsout;
	} else {
		echo "<TR>\n<TH COLSPAN=8 BGCOLOR=\"lightgreen\"><FONT SIZE =+1>ALL MONITORED HOSTS UP</FONT></TH>\n</TR>\n";
	}
	echo "</TABLE>\n";

	echo "<P>\n";

	echo "<FONT SIZE=-1>\n";
	echo "<TABLE BORDER=0 celspacing=2 width=80%>\n";
	echo "<TR>\n<TH COLSPAN=3><TH COLSPAN=2><FONT SIZE =+1>Service Status</FONT><BR><FONT SIZE=-1>(down hosts show no services)</FONT></TH><TH>";

	if($services_ok) { echo "<FONT COLOR=\"green\">$services_ok OK</FONT> "; }
	if($services_warning) { echo "<FONT COLOR=\"orange\">$services_warning Warning</FONT> "; }
	if($services_critical) { echo "<FONT COLOR=\"red\">$services_critical Critical</FONT> "; }
	if($services_pending) { echo "<FONT COLOR=\"grey\">$services_pending Pending</FONT> "; }
	if($service_unknown) { echo "<FONT COLOR=\"yellow\">$services_unknown Unknown</FONT> "; }
        echo "\n</TH></TR>\n";
	if(strlen($servicesout_warning) || strlen($servicesout_error)) {
		echo "<TR bgcolor=\"lightgrey\">\n";
		echo "\t<TH>Host</TH><TH>Service</TH><TH>Status</TH><TH>Last Change</TH><TH>Duration</TH><TH>Status Information</TH>\n";
		echo "</TR>\n";
		echo $servicesout_error;
		echo $servicesout_warning;
		echo "</TABLE>\n";
	} else {
		echo "<TR>\n<TH COLSPAN=8 BGCOLOR=\"lightgreen\"><FONT SIZE =+1>ALL MONITORED SERVICES OK</FONT></TH>\n</TR>\n";
		echo "</FONT>\n";
	}

	if(strlen($notifications_off)) {
		echo "<P>\n";
		echo "<TABLE BORDER=0 celspacing=2 width=80%>\n";
		echo "<TR>\n<TH COLSPAN=8><FONT SIZE =+1>Hosts With Notifications Turned off</FONT></TH>\n</TR>\n";
		echo "<TR bgcolor=\"yellow\">\n";
		echo "<TD>".$notifications_off."</TD>\n";
		echo "</TR></TABLE>\n";
	}

	echo "<P>";

	echo "<TABLE BORDER=0 celspacing=2 width=80%>\n";
	echo "<TR>\n<TH COLSPAN=8><FONT SIZE =+1>Nagios Monitor Status</FONT></TH>\n</TR>\n";
	echo "<TR>\n";
	echo "<TD>Process ID: </TD>\n". "<TD>".$pstatus["nagios_pid"]."</TD>\n";
	echo "</TR>\n";
	echo "<TR>\n";
	echo "<TD>Start Time: </TD>\n". "<TD>".$pstatus["created"]."</TD>\n";
	echo "</TR>\n";
	echo "<TR>\n";
	echo "<TD>Check Status: </TD>\n". "<TD>".$pstatus["nagios_status"]."</TD>\n";
	echo "</TR>\n";
	echo "<TR>\n";
	echo "<TD>Webpage refresh interval (s): </TD>\n". "<TD>90</TD>\n";
	echo "</TR>\n";
	echo "</TABLE>\n";
?>

	</CENTER>
	</BODY>
	</HTML>

