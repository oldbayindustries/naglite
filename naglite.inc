<?php
/*
**     This program is free software: you can redistribute it and/or modify
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
/*	This code is based on the nag-lite project found on http://nagiosexchange.org
**	
**	this code is tested on nagios version 2 and 3
**
**	Bo Larsen, ha9bal@gmail.com
*/
?>



<?php
	$cgi_ini='/opt/local/etc/nagios/cgi.cfg';

	$nagios_cfg = ini_getitem($cgi_ini, "main_config_file");

	$nagios_check_command = ini_getitem($cgi_ini, "nagios_check_command");

	$refresh_rate = ini_getitem($cgi_ini, "refresh_rate");

	$status_file = ini_getitem($nagios_cfg, "status_file");

	$status_lines = array();


	function ini_getitem($filename,$item,$length=1024,$notrim=false) { 
		$fp = fopen($filename,"r");
    
    		if(!$fp) {
       			return false;
    		}
    
		while (!feof($fp)) { 
		        $fileline = fgets($fp,$length);
        		if ( substr($fileline,0,strlen($item)+1) == $item . "=" )
            		if ( $notrim )
               			return substr($fileline,strlen($item)+1,strlen($fileline)-strlen($item));
            		else 
                		return trim(substr($fileline,strlen($item)+1,strlen($fileline)-strlen($item)));
    		}
    		fclose($fp);
    		return false;
	}


	function read_status_file($status_file) {
 		$lines = array();

  		$fh = @fopen($status_file, "r");

  		if (!$fh) { echo "<CENTER><FONT COLOR=\"red\" SIZE=+2>Failed to open Status file: ", $status_file, " ", $nagios_cfg, "</FONT></CENTER><BR>\n"; }

  		while ($fh && !feof($fh)) {
       			$lines[] = fgets($fh, 4096);
  		}
  		fclose($fh);

  		return $lines;
	}

	$mypipe = popen($nagios_check_command, "r");
	$nagios_status = fgets($mypipe, 1024);
	pclose($mypipe);

	$pstatus["nagios_status"] = $nagios_status;


	function parse_status_array($status_lines) {

		global $hlist;
		global $pstatus;

		global $services_ok;
		global $services_warning;
		global $services_pending;
		global $services_critical;
		global $services_unknown;

                $rec = false;
                
		foreach ($status_lines as $line) {
        		if (strpos($line, "#") === false){

				if (strpos($line, "{")) { 
					list($rectype,$value) = split(" ",$line);
                                        $host = "";
					$description = "";
                                        $plugin_output = "";
					$rec = true; 
                                        
				}
				else {
                                	if (strpos($line, "}")) {
						#echo trim($host)."-".trim($rectype)."-duration-".trim($duration)."<br />\n";
						
						if ($rectype == "info") {
							$pstatus["version"] = $version;
							$pstatus["created"] = $created;
						}

						if (($rectype == "programstatus") || ($rectype == "program")) {
							$pstatus["nagios_pid"] = $nagios_pid;
							$pstatus["program_start"] = $program_start;
							$pstatus["last_command_check"] = $last_command_check;
							$pstatus["enable_notifications"] = ($enable_notifications) ? "On" : "Off";
						}

						if (($rectype == "hoststatus") || ($rectype == "host")) {
							$hlist[$host_name]["host"]["host_name"] = $host_name;
							$hlist[$host_name]["host"]["status"] = $status;
							$hlist[$host_name]["host"]["last_check"] = $last_check;
							$hlist[$host_name]["host"]["last_state_change"] = $last_state_change;
							$hlist[$host_name]["host"]["problem_has_been_acknowledged"] = $problem_has_been_acknowledged;
							$hlist[$host_name]["host"]["time_up"] = $time_up;
							$hlist[$host_name]["host"]["time_down"] = $time_down;
							$hlist[$host_name]["host"]["time_unreachable"] = $time_unreachable;
							$hlist[$host_name]["host"]["last_notification"] = $last_notification;
							$hlist[$host_name]["host"]["current_notification_number"] = $current_notification_number;
							$hlist[$host_name]["host"]["notifications_enabled"] = $notifications_enabled;
							$hlist[$host_name]["host"]["event_handler_enabled"] = $event_handler_enabled;
							$hlist[$host_name]["host"]["checks_enabled"] = $checks_enabled;
							$hlist[$host_name]["host"]["flap_detection_enabled"] = $flap_detection_enabled;
							$hlist[$host_name]["host"]["is_flapping"] = $is_flapping;
							$hlist[$host_name]["host"]["percent_state_change"] = $percent_state_change;
							$hlist[$host_name]["host"]["scheduled_downtime_depth"] = $scheduled_downtime_depth;
							$hlist[$host_name]["host"]["failure_prediction_enabled"] = $failure_prediction_enabled;
							$hlist[$host_name]["host"]["process_performance_data"] = $process_performance_data;
							$hlist[$host_name]["host"]["plugin_output"] = $plugin_output;
							$hlist[$host_name]["host"]["last_update"] = $last_update;
							$hlist[$host_name]["host"]["duration"] = $duration;
						}

                                                if (($rectype == "servicestatus") || ($rectype == "service")) {
					
							if($status == "OK") {
								$services_ok++;
							} else if($status == "WARNING") {
								$services_warning++;
							} else if($status == "PENDING") {
								$services_pending++;
							} else if($status == "CRITICAL") {
								$services_critical++;
							} else {
								$services_unknown++;
							}


							if($status != "OK" && $status != "PENDING") { # We kill da memory - only store ! ok services
								$hlist[$host_name]["service"][$description]["host_name"] = $host_name;
								$hlist[$host_name]["service"][$description]["description"] = $description;
								$hlist[$host_name]["service"][$description]["status"] = $status;
								$hlist[$host_name]["service"][$description]["current_attempt"] = $current_attempt;
								$hlist[$host_name]["service"][$description]["max_attempts"] = $max_attempts;
								$hlist[$host_name]["service"][$description]["state_type"] = $state_type;
								$hlist[$host_name]["service"][$description]["last_check"] = $last_check;
								$hlist[$host_name]["service"][$description]["next_check"] = $next_check;
								$hlist[$host_name]["service"][$description]["check_type"] = $check_type;
								$hlist[$host_name]["service"][$description]["checks_enabled"] = $checks_enabled;
								$hlist[$host_name]["service"][$description]["accept_passive_service_checks"] = $accept_passive_service_checks;
								$hlist[$host_name]["service"][$description]["event_handler_enabled"] = $event_handler_enabled;
								$hlist[$host_name]["service"][$description]["last_state_change"] = $last_state_change;
								$hlist[$host_name]["service"][$description]["problem_has_been_acknowledged"] = $problem_has_been_acknowledged;
								$hlist[$host_name]["service"][$description]["last_hard_state"] = $last_hard_state;
								$hlist[$host_name]["service"][$description]["time_ok"] = $time_ok;
								$hlist[$host_name]["service"][$description]["time_unknown"] = $time_unknown;
								$hlist[$host_name]["service"][$description]["time_warning"] = $time_warning;
								$hlist[$host_name]["service"][$description]["time_critical"] = $time_critical;
								$hlist[$host_name]["service"][$description]["last_notification"] = $last_notification;
								$hlist[$host_name]["service"][$description]["current_notification_number"] = $current_notification_number;
								$hlist[$host_name]["service"][$description]["notifications_enabled"] = $notifications_enabled;
								$hlist[$host_name]["service"][$description]["latency"] = $latency;
								$hlist[$host_name]["service"][$description]["execution_time"] = $execution_time;
								$hlist[$host_name]["service"][$description]["flap_detection_enabled"] = $flap_detection_enabled;
								$hlist[$host_name]["service"][$description]["is_flapping"] = $is_flapping;
								$hlist[$host_name]["service"][$description]["percent_state_change"] = $percent_state_change;
								$hlist[$host_name]["service"][$description]["scheduled_downtime_depth"] = $scheduled_downtime_depth;
								$hlist[$host_name]["service"][$description]["failure_prediction_enabled"] = $failure_prediction_enabled;
								$hlist[$host_name]["service"][$description]["process_performance_data"] = $process_performance_data;
								$hlist[$host_name]["service"][$description]["obsess_over_service"] = $obsess_over_service;
								$hlist[$host_name]["service"][$description]["plugin_output"] = $plugin_output;
								$hlist[$host_name]["service"][$description]["last_update"] = $last_update;
								$hlist[$host_name]["service"][$description]["duration"] = $duration;
							}
						} 

						$rec = false;
                                	}

                                	if ($rec) {
                       				list($key,$value) = split("=",$line,2);

						if ($rectype == "info") {
							if (strpos($key, "version") == 1) {
                                                                $version = trim($value);
                                                        }
							if (strpos($key, "created") == 1) {
                                                                $created = strftime ("%b %d %Y %H:%M:%S", trim($value));
                                                        }
						}

						if (($rectype == "programstatus") || ($rectype == "program")) {
							if (strpos($key, "nagios_pid") == 1) {
								$nagios_pid = trim($value);
							}
							if (strpos($key, "program_start") == 1) {
								$program_start = strftime ("%b %d %Y %H:%M:%S", trim($value));
							}
							if (strpos($key, "last_command_check") == 1) {
								$last_command_check = strftime ("%b %d %Y %H:%M:%S", trim($value));
							}
							if (strpos($key, "enable_notifications") == 1) {
								$enable_notifications = trim($value);
							}
						}

						if (($rectype == "hoststatus") || ($rectype == "host")) {
                                                	if (strpos($key, "host_name") == 1) {
                                                        	$host_name = trim($value);
                                                	}
	                                                if (strpos($key, "current_state") == 1) {
        	                                                switch (trim($value)) {
									case '0': 	$status = "UP";
											break;
									case '1':	$status = "DOWN";
											break;
									case '2':	$status = "UNREACHABLE";
											break;
									default : 	$status = "PENDING";
								}	
                	                                }
							if (strpos($key, "last_update") == 1) {				
								$last_update = strftime ("%b %d %Y %H:%M:%S", trim($value));
							}
							if (strpos($key, "last_check") == 1) {
								$last_check = strftime ("%b %d %Y %H:%M:%S", trim($value));
                                                        }
                                                        if (strpos($key, "last_state_change") == 1) {
								$last_state_change = trim($value);
                                                        }
                                                        if (strpos($key, "problem_has_been_acknowledged") == 1) {
 								$problem_has_been_acknowledged = trim($value);
                                                        }
                                                        if (strpos($key, "time_up") == 1) {
 								$time_up = trim($value);
                                                        }
                                                        if (strpos($key, "time_down") == 1) {
 								$time_down = trim($value);
                                                        }
                                                        if (strpos($key, "time_unreachable") == 1) {
 								$time_unreachable = trim($value);
                                                        }
                                                        if (strpos($key, "last_notification") == 1) {
 								$last_notification = strftime ("%b %d %Y %H:%M:%S", trim($value));
                                                        }
                                                        if (strpos($key, "current_notification_number") == 1) {
 								$current_notification_number = trim($value);
                                                        }
                                                        if (strpos($key, "notifications_enabled") == 1) {
 								$notifications_enabled = trim($value);
                                                        }
                                                        if (strpos($key, "event_handler_enabled") == 1) {
 								$event_handler_enabled = trim($value);
                                                        }
                                                        if (strpos($key, "checks_enabled") == 1) {
 								$checks_enabled = trim($value);
                                                        }
                                                        if (strpos($key, "flap_detection_enabled") == 1) {
 								$flap_detection_enabled = trim($value);
                                                        }
                                                        if (strpos($key, "is_flapping") == 1) {
 								$is_flapping = trim($value);
                                                        }
                                                        if (strpos($key, "percent_state_change") == 1) {
 								$percent_state_change = trim($value);
                                                        }
                                                        if (strpos($key, "scheduled_downtime_depth") == 1) {
 								$scheduled_downtime_depth = trim($value);
                                                        }
                                                        if (strpos($key, "failure_prediction_enabled") == 1) {
 								$failure_prediction_enabled = trim($value);
                                                        }
                                                        if (strpos($key, "process_performance_data") == 1) {
 								$process_performance_data = trim($value);
                                                        }
                                                        if (strpos($key, "plugin_output") == 1) {
 								$plugin_output = $value;
							}
	                                                if (strpos($key, "last_state_change") == 1) {
        	                                                $duration = gmstrftime("%H:%M:%S", time(0) - $value);
                	                                        $last_state_change = strftime ("%b %d %Y %H:%M:%S", $last_state_change);
                        	                        }
						}
												
						if (($rectype == "servicestatus") || ($rectype == "service")) {
                                                        if (strpos($key, "host_name") == 1) {
                                                                $host_name = trim($value);
                                                        }
                                                        if (strpos($key, "service_description") == 1) {
                                                                $description = trim($value);
                                                        }
                                                        if (strpos($key, "current_state") == 1) {
                                                                switch (trim($value)) {
                                                                        case '0':       $status = "OK";
                                                                                        break;
                                                                        case '1':       $status = "WARNING";
                                                                                        break;
                                                                        case '2':       $status = "CRITICAL";
                                                                                        break;
									case '3':	$status = "UNKNOWN";
											break;
									default :       $status = "PENDING";
                                                                }
                                                        }
                                                        if (strpos($key, "last_update") == 1) {
                                                                $last_update = strftime ("%b %d %Y %H:%M:%S", trim($value));
                                                        }
                                                        if (strpos($key, "last_check") == 1) {
                                                                $last_check = strftime ("%b %d %Y %H:%M:%S", trim($value));
                                                        }
                                                        if (strpos($key, "last_state_change") == 1) {
                                                                $last_state_change = trim($value);
                                                        }
 							if (strpos($key, "current_attempt") == 1) {
								$current_attempt = trim($value);
                                                        }
                                                        if (strpos($key, "max_attempts") == 1) {
 								$max_attempts = trim($value);
                                                        }
                                                        if (strpos($key, "state_type") == 1) {
 								$state_type = trim($value);
                                                        }
/*                                                        if (strpos($key, "next_check") == 1) {
								$next_check = strftime ("%b %d %Y %H:%M:%S", trim($value));
                                                        }
*/                                                        if (strpos($key, "check_type") == 1) {
								$check_type = trim($value);
                                                        }
                                                        if (strpos($key, "checks_enabled") == 1) {
 								$checks_enabled = trim($value);
                                                        }
                                                        if (strpos($key, "accept_passive_service_checks") == 1) {
 								$accept_passive_service_checks = trim($value);
                                                        }
                                                        if (strpos($key, "event_handler_enabled") == 1) {
 								$event_handler_enabled = trim($value);
                                                        }
                                                        if (strpos($key, "problem_has_been_acknowledged") == 1) {
 								$problem_has_been_acknowledged = trim($value);
                                                        }
                                                        if (strpos($key, "last_hard_state") == 1) {
 								$last_hard_state = trim($value);
                                                        }
                                                        if (strpos($key, "time_ok") == 1) {
 								$time_ok = trim($value);
                                                        }
                                                        if (strpos($key, "time_unknown") == 1) {
 								$time_unknown = trim($value);
                                                        }
                                                        if (strpos($key, "time_warning") == 1) {
 								$time_warning = trim($value);
                                                        }
                                                        if (strpos($key, "time_critical") == 1) {
 								$time_critical = trim($value);
                                                        }
                                                        if (strpos($key, "last_notification") == 1) {
 								$last_notification = strftime("%b %d %Y %H:%M:%S", trim($value));
                                                        }
                                                        if (strpos($key, "current_notification_number") == 1) {
 								$current_notification_number = trim($value);
                                                        }
                                                        if (strpos($key, "notifications_enabled") == 1) {
 								$notifications_enabled = trim($value);
                                                        }
                                                        if (strpos($key, "latency") == 1) {
 								$latency = trim($value);
                                                        }
                                                        if (strpos($key, "execution_time") == 1) {
 								$execution_time = trim($value);
                                                        }
                                                        if (strpos($key, "flap_detection_enabled") == 1) {
 								$flap_detection_enabled = trim($value);
                                                        }
                                                        if (strpos($key, "is_flapping") == 1) {
 								$is_flapping = trim($value);
                                                        }
                                                        if (strpos($key, "percent_state_change") == 1) {
 								$percent_state_change = trim($value);
                                                        }
                                                        if (strpos($key, "scheduled_downtime_depth") == 1) {
 								$scheduled_downtime_depth = trim($value);
                                                        }
                                                        if (strpos($key, "failure_prediction_enabled") == 1) {
 								$failure_prediction_enabled = trim($value);
                                                        }
                                                        if (strpos($key, "process_performance_data") == 1) {
 								$process_performance_data = trim($value);
                                                        }
                                                        if (strpos($key, "obsess_over_service") == 1) {
 								$obsess_over_service = trim($value);
                                                        }
                                                        if (strpos($key, "plugin_output") == 1) {
								$plugin_output = $value;
							}
                                                        if (strpos($key, "last_state_change") == 1) {
                                                                $duration = gmstrftime("%H:%M:%S", time(0) - $value);
                                                                $last_state_change = strftime ("%b %d %Y %H:%M:%S", $last_state_change);
                                                        }
						}
                                	}
				}
        		}
    		}
	}


?>
