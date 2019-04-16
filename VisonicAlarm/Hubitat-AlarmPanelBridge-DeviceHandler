/**
 *  Copyright 2017 Chris Charles
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 *
 *  Alarm Bridge Controller
 *
 *  Author: Chris Charles (cjcharles0)
 *  Date: 2017-02-10
 *  Version: 1.04
 */

import groovy.json.JsonSlurper

metadata
{
	definition (name: "Alarm Panel Bridge", namespace: "cjcharles0", author: "Chris Charles")
	{
		capability "Refresh"
		capability "Configuration"
		capability "Alarm"
		capability "Polling"
		
		attribute "device.status", "string"
		attribute "armaway", "string"
		attribute "armhome", "string"
		attribute "disarm", "string"
		attribute "alarm", "enum"
		attribute "lastaction", "string"
		
		command "AlarmArmAway"
		command "AlarmArmHome"
		command "AlarmDisarm"
        command "AlarmTrigger"
        
		command "ZoneCreateChildDevices"
		command "ZoneRemoveChildDevices"
		command "ZoneTestFunction"
        
        command "childOff"
        command "childOn"
        command "childRefresh"
		command "outputoff"
		command "outputon"
        
        (1..8).each { n ->
			command "on$n"
			command "off$n"
		}
        
		command "stop"
		command "off"
		command "strobe"
		command "siren"
		command "both"
	}

	simulator {
	}
	
	preferences {
        input name: "ip", type: "string", title:"Alarm IP Address", description: "e.g. 192.168.1.10", required: true, displayDuringSetup: true
        input name: "alarmtriggermethod", type: "enum", title: "Method to trigger alarm", options: ["Serial", "IO"], description: "Default D7 if unsure", required: true, displayDuringSetup: true
        input name: "prename", type: "string", title:"Add X before zone name", description: "e.g. 'Zone' would give 'Zone Kitchen'", required: false, displayDuringSetup: true
        input name: "postname", type: "string", title:"Add X after zone name", description: "e.g. 'Zone' would give 'Kitchen Zone'", required: false, displayDuringSetup: true
        input name: "inactivityseconds", type: "string", title:"Motion sensor inactivity timeout", description: "override the default of 20s (60s max)", required: false, displayDuringSetup: false
        input name: "password", type: "password", title:"Password for wemos (leave blank)", required:false, displayDuringSetup:false
	}

	tiles (scale: 2)
	{	  
		multiAttributeTile(name:"AlarmStatus", type: "generic", width: 6, height: 3, decoration:"flat")
		{
			tileAttribute ("device.status", key: "PRIMARY_CONTROL")
			{
				attributeState "disarmed", label:'${name}', icon: "st.security.alarm.off", backgroundColor: "#505050"
				attributeState "home", label:'${name}', icon: "st.Home.home4", backgroundColor: "#00BEAC"
				attributeState "away", label:'${name}', icon: "st.security.alarm.on", backgroundColor: "#008CC1"
				attributeState "alarm", label:'${name}', icon: "st.security.alarm.on", backgroundColor: "#d44556"
			}
			tileAttribute("device.events", key: "SECONDARY_CONTROL", wordWrap: true)
			{
				attributeState("default", label:'${currentValue}')
			}
		}
		
		standardTile("VArmAway", "armaway", height: 2, width:2, decoration:"flat", inactiveLabel: false)
		{
			state "inactive", label:"Away", action:"AlarmArmAway", backgroundColor:"#D8D8D8"
			state "changing", label:"Arming Away", action:"", backgroundColor:"#FF9900"
			state "active", label:"Armed Away", action:"", backgroundColor:"#00CC00"
		}
		standardTile("VArmHome", "armhome", height: 2, width: 2, decoration:"flat", inactiveLabel: false)
		{
			state "inactive", label:"Home", action:"AlarmArmHome", backgroundColor:"#D8D8D8"
			state "changing", label:"Arming Home", action:"", backgroundColor:"#FF9900"
			state "active", label:"Armed Home", action:"", backgroundColor:"#00CC00"
		}
		standardTile("VDisarm", "disarm", height: 2, width: 2, decoration:"flat", inactiveLabel: false)
		{
			state "inactive", label:"Disarm", action:"AlarmDisarm", backgroundColor:"#D8D8D8"
			state "changing", label:"Disarming", action:"", backgroundColor:"#FF9900"
			state "active", label:"Disarmed", action:"", backgroundColor:"#00CC00"
		}
		
        //This will create a tile for all zones up to 30 (which should cover most boards including Wired ones)
        //We can have all of these tiles, but not display them
		(1..30).each { n ->
			valueTile("zonename$n", "panelzonename$n", height: 1, width: 2) {
				state "default", label:'${currentValue}', backgroundColor:"#FFFFFF"}
			standardTile("zone$n", "panelzone$n", height: 1, width: 1) {
				state "inactive", label:"Inactive", action:"", icon:""
				state "active", label:"Active", action:"", icon:"", backgroundColor:"#00CC00"
				state "closed", label:"Closed", action:"", icon:""
				state "open", label:"Open", action:"", icon:"", backgroundColor:"#00CC00"
				state "bypass", label:"Bypass", action:"", icon:"", backgroundColor:"#CCCC00"
				}
		}
        
        //This will create an output tile as needed for control, not relevant to Visonic
		(1..8).each { n ->
			valueTile("outputname$n", "paneloutputname$n", height: 1, width: 2) {
				state "default", label:'${currentValue}', backgroundColor:"#FFFFFF"}
			standardTile("output$n", "paneloutput$n", height: 1, width: 1) {
				state "on", label:"On", action:"off$n", icon:"", backgroundColor:"#00CC00"
				state "off", label:"Off", action:"on$n", icon:""
				}
		}
		
		standardTile("refresh", "device.refresh", inactiveLabel: false, decoration: "flat", width: 1, height: 1)
		{
			state "default", label:"", action:"refresh", icon:"st.secondary.refresh"
		}
	  
		standardTile("configure", "device.configure", inactiveLabel: false, width: 1, height: 1, decoration: "flat")
		{
			state "configure", label:'', action:"configure", icon:"st.secondary.configure"
		}

		valueTile("ip", "ip", decoration: "flat", width: 2, height: 1)
		{
			state "ip", label:'ST IP Addr:\r\n${currentValue}'
		}
		
		standardTile("createtile", "device.createzonedevices", inactiveLabel: false, decoration: "flat", width: 2, height: 1)
		{
			state "default", label:'Create Zone Devices', action:"ZoneCreateChildDevices", icon: "st.unknown.zwave.remote-controller"
		}
		standardTile("removetile", "device.removezonedevices", inactiveLabel: false, decoration: "flat", width: 2, height: 1)
		{
			state "default", label:'Remove Zone Devices', action:"ZoneRemoveChildDevices", icon: "st.samsung.da.washer_ic_cancel"
		}
		
		standardTile("testtile", "device.testfunction", inactiveLabel: false, decoration: "flat", width: 2, height: 1)
		{
			state "default", label:'test_function', action:"ZoneTestFunction"
		}
		
		//This alarm tile is used so that the state of the alarm is recorded in a useful existing ST capability.
		//This allows you to trigger other things in CoRE (or similar) when the state changes, or to trigger state changes
		//To use this, you can make a CoRE trigger if 'Alarm' state changes to 'Both' (this will tell you the alarm is going off)
		//or if 'Alarm' state changes to 'Off' (this will tell you the alarm has been disarmed).
		//Alternatively if you set the 'Alarm' capability to 'Siren' then it will arm Away.
		standardTile("alarm", "device.alarm", inactiveLabel: false, width: 2, height: 1, decoration: "flat")
		{
			state "off", label:'Alarm state =\r\nOff (Disarmed)'
			state "strobe", label:'Alarm state =\r\nStrobe (Home)'
			state "siren", label:'Alarm state =\r\nSiren (Away)'
			state "both", label:'Alarm state =\r\nBoth (Alarm)'
		}
	}

	main(["AlarmStatus"])
	details(["AlarmStatus", "VArmAway", "VArmHome", "VDisarm",
			 
             // Add all your zones here (zonenameX first then zoneX)- e.g. add "zonename29", "zone29" if you want to display zone 29 in the alarm panel
			 "zonename1", "zone1", "zonename2", "zone2", "zonename3", "zone3", "zonename4", "zone4",
			 "zonename5", "zone5", "zonename6", "zone6", "zonename7", "zone7", "zonename8", "zone8", 
			 
             // Add any outputs here by uncommenting the row and then add in the same format as for zones, but without output rather than zone
             "outputname1", "output1",
             
			 "refresh", "configure", "ip", "alarm", "createtile", "removetile"
			 //, "testtile" //If you have trouble creating your child zones you can uncomment the start of this line (remove // before the comma)
			 				//After pressing the "test_function" button on your phone you can re-comment it, to tidy up your alarm panel device.
							//This is only needed if you have trouble creating your zones.
			 ])
}

def AlarmArmAway()
{
	//Send an arm away command to the alarm and log that it is changing
	log.debug "armaway()"
	sendEvent(name: "armaway", value: "changing")
	sendEvent(name: "armhome", value: "inactive", displayed: false)
	sendEvent(name: "disarm", value: "inactive", displayed: false)
	getAction("/armaway")
}

def AlarmArmHome()
{
	//Send an arm home command to the alarm and log that it is changing
	log.debug "armhome()"
	sendEvent(name: "armaway", value: "inactive", displayed: false)
	sendEvent(name: "armhome", value: "changing")
	sendEvent(name: "disarm", value: "inactive", displayed: false)
	getAction("/armhome")
}

def AlarmDisarm()
{
	//Send a disarm command to the alarm and log that it is changing
	log.debug "disarm()"
	sendEvent(name: "armaway", value: "inactive", displayed: false)
	sendEvent(name: "armhome", value: "inactive", displayed: false)
	sendEvent(name: "disarm", value: "changing")
	getAction("/disarm")
}

def AlarmTrigger() {
	//Try to manually trigger the alarm (there are several ways possible - some working!)
	log.debug "Trying to trigger the alarm by the ${alarmtriggermethod} method"
    if (alarmtriggermethod != null) {
    	def outputinfo = ""
    	switch (alarmtriggermethod) {
        	case ["IO"]:
            	outputinfo = "?method=IO"
                break
            case ["Serial"]:
            	outputinfo = "?method=Serial"
                break
        }
    	getAction("/alarm${outputinfo}")
    }
}

def ZoneCreateChildDevices()
{
	//This will create child devices
	//ZoneRemoveChildDevices() //Cant do this here as timing gets messed up
	log.debug "Requesting List of Alarm Zones"
	getAction("/getzonenames")
}

def ZoneRemoveChildDevices()
{
	//This will remove child devices
	log.debug "Removing Child Devices"
	try
	{
		getChildDevices()?.each
		{
			try
			{
				deleteChildDevice(it.deviceNetworkId)
			}
			catch (e)
			{
				log.debug "Error deleting ${it.deviceNetworkId}, probably locked into a SmartApp: ${e}"
			}
		}
	}
	catch (err)
	{
		log.debug "Either no children exist or error finding child devices for some reason: ${err}"
	}
}

def ZoneTestFunction()
{
	//This function just creates a test device due to some random problems that exist when creating children
	//log.debug "Got here inside the test function"
	def hub = location.hubs[0]
	if (device.currentValue("createzonedevices") == "cancreatezonedevices")
	{
		log.debug "state correct"
	}
	
	try
	{
		def curdevice = getChildDevices()?.find { it.deviceNetworkId == "alarmchildzonetest"}
		if (curdevice)
		{
			//Do nothing as we already have a test device
		}
		else
		{
			addChildDevice("hubitat", "Virtual Motion Sensor", "alarmchildzonetest", [name: "alarm zone test device", isComponent: false])
		}
	} 
	catch (e)
	{
		log.error "Couldnt find device, probably doesn't exist so safe to add a new one: ${e}"
		addChildDevice("hubitat", "Virtual Motion Sensor", "alarmchildzonetest", [name: "alarm zone test device", isComponent: false])
	}
}

//This will configure the Wemos to talk to ST
def configure()
{
	def cmds = []
	def hub = location.hubs[0]
	log.debug "Configuring Alarm (getting zones+types, configuring IP/port/timeout)"
	cmds << getAction("/status")
	
	def requeststring = "/config?ip_for_st=${hub.getDataValue("localIP")}&port_for_st=${hub.getDataValue("localSrvPortTCP")}"
	
    if (inactivityseconds?.isInteger()) {
    	//Inactivityseconds is both populated and an integer, so lets send it to the Wemos
        requeststring = requeststring + "&inactivity_seconds=${settings.inactivityseconds}"
    }
	
	//log.debug requeststring
	//Send the details to SmartThings
	cmds << getAction(requeststring)
	return cmds
}

def refresh()
{
	log.debug "refresh()"
	def hub = location.hubs[0]
	//SendEvents should be before any getAction, otherwise getAction does nothing
	sendEvent(name: "ip", value: hub.getDataValue("localIP")+"\r\nPort: "+hub.getDataValue("localSrvPortTCP"), displayed: false)
	//Now refresh Alarm status
	getAction("/refresh")
	//getAction("/getzonenames")
}

def installed()
{
	log.debug "installed()"
	//configure()
}

def updated()
{
	log.debug "updated()"
	configure()
}

def ping()
{
	log.debug "ping()"
	getAction("/ping")
}

//These functions are needed due to the alarm device capability and hence will server to arm/disarm the alarm (though unlikely to be called)
def stop()
{
	AlarmDisarm()
}

def off()
{
	AlarmDisarm()
}

def strobe()
{
	AlarmArmHome()
}

def siren()
{
	AlarmArmAway()
}

def both()
{
	AlarmTrigger()
}

def parse(description) {

	def map = [:]
	def events = []
	def cmds = []
	def hub = location.hubs[0]
	
	if(description == "updated") return
	def descMap = parseDescriptionAsMap(description)

	def body = new String(descMap["body"].decodeBase64())

	def slurper = new JsonSlurper()
	def result = slurper.parseText(body)
	
	log.debug result
	
	//Received an alarm stat string so update tile status but dont display it since the event string below will be logged
	if (result.containsKey("stat_str"))
	{
		switch (result.stat_str)
		{
			case ["Disarmed", "Disarm", "Ready"]:
				sendEvent(name: "disarm", value: "active", displayed: false)
				sendEvent(name: "armaway", value: "inactive", displayed: false)
				sendEvent(name: "armhome", value: "inactive", displayed: false)
				sendEvent(name: "status", value: "disarmed", displayed: false)
				sendEvent(name: "alarm", value: "off", displayed: false)
				log.debug "Disarmed Status found"
				break
				
			case ["Not Ready"]:
				sendEvent(name: "disarm", value: "inactive", displayed: false)
				sendEvent(name: "armaway", value: "inactive", displayed: false)
				sendEvent(name: "armhome", value: "inactive", displayed: false)
				sendEvent(name: "status", value: "disarmed", displayed: false)
				sendEvent(name: "alarm", value: "off", displayed: false)
				log.debug "Not-ready Status found"
				break

			case ["Armed Away", "Arm Away", "Quick Arm Away"]:
				sendEvent(name: "disarm", value: "inactive", displayed: false)
				sendEvent(name: "armaway", value: "active", displayed: false)
				sendEvent(name: "armhome", value: "inactive", displayed: false)
				sendEvent(name: "status", value: "away", displayed: false)
				sendEvent(name: "alarm", value: "siren", displayed: false)
				log.debug "Armed Away Status found"
				break

			case ["Armed Home", "Arm Home", "Quick Arm Home"]:
				sendEvent(name: "disarm", value: "inactive", displayed: false)
				sendEvent(name: "armaway", value: "inactive", displayed: false)
				sendEvent(name: "armhome", value: "active", displayed: false)
				sendEvent(name: "status", value: "home", displayed: false)
				sendEvent(name: "alarm", value: "strobe", displayed: false)
				log.debug "Armed Home Status found"
				break

			case ["Exit Delay"]:
				sendEvent(name: "disarm", value: "inactive", displayed: false)
				sendEvent(name: "armaway", value: "changing")
				sendEvent(name: "armhome", value: "inactive", displayed: false)
				log.debug "Exit Delay Status found"
				break
 
			case ["Delay Alarm", "Confirm Alarm"]:
				sendEvent(name: "disarm", value: "inactive", displayed: false)
				sendEvent(name: "armaway", value: "inactive", displayed: false)
				sendEvent(name: "armhome", value: "inactive", displayed: false)
				sendEvent(name: "status", value: "alarm", displayed: false)
				sendEvent(name: "alarm", value: "both", displayed: false)
				log.debug "Alarm Status found - Uh Oh!!"
				break

			default:
				log.debug "Unknown Alarm state received = ${result.stat_str}"
				break
		}
	}
	
	//If we receive a key containing 'stat_update_from' then it is an alarm status so add it to the event log and update tile
	if (result.containsKey("stat_update_from"))
	{
		def dateTime = new Date()
		def sensorStateChangedDate = dateTime.format("yyyy-MM-dd HH:mm", location.timeZone)
		def status_string = result.stat_str + " by " + result.stat_update_from + " at " + sensorStateChangedDate
		//Send the status string that we have built
		sendEvent(name: "events", value: "${status_string}", displayed: true, isStateChange: true)
	}
	
	if (result.containsKey("zone_status"))
	{
		//Got a zone status so first try to find the correct child device
		def curdevice = getChildDevices()?.find { it.deviceNetworkId == "alarmchildzone"+result.zone_id}
		//Now switch based on what has happened
		switch (result.zone_status)
		{
			case "Violated (Motion)":
				//log.debug "Got Active motion zone: " + result.zone_id + ", which is called - " + curdevice
				sendEvent(name: "panelzone"+result.zone_id, value: "active", displayed: false, isStateChange: true)
				curdevice?.sendEvent(name: "motion", value: "active")
				break

			case "No Motion":
				//log.debug "Got Inactive motion zone: " + result.zone_id + ", which is called - " + curdevice
				sendEvent(name: "panelzone"+result.zone_id, value: "inactive", displayed: false, isStateChange: true)
				curdevice?.sendEvent(name: "motion", value: "inactive")
				break

			case "Open":
				//log.debug "Got Open zone: " + result.zone_id + ", which is called - " + curdevice
				sendEvent(name: "panelzone"+result.zone_id, value: "open", displayed: false, isStateChange: true)
				curdevice?.sendEvent(name: "contact", value: "open")
				break

			case "Closed":
				//log.debug "Got Closed zone: " + result.zone_id + ", which is called - " + curdevice
				sendEvent(name: "panelzone"+result.zone_id, value: "closed", displayed: false, isStateChange: true)
				curdevice?.sendEvent(name: "contact", value: "closed")
				break
				
			case "Bypassed (Motion)":
            	//This is a zone which is bypassed and currently inactive for motion
				//log.debug "Got Bypassed zone: " + result.zone_id + ", which is called - " + curdevice
				sendEvent(name: "panelzone"+result.zone_id, value: "bypass", displayed: false, isStateChange: true)
				curdevice?.sendEvent(name: "motion", value: "inactive")
				break

			case "Bypassed":
            	//This is a zone which is bypassed and currently closed
				//log.debug "Got Bypassed zone: " + result.zone_id + ", which is called - " + curdevice
				sendEvent(name: "panelzone"+result.zone_id, value: "bypass", displayed: false, isStateChange: true)
				curdevice?.sendEvent(name: "contact", value: "closed")
				break

            case "Tamper":
				//log.debug "Got Tamper for zone: " + result.zone_id + ", which is called - " + curdevice
                //We'll set it to open for now, since at least that gives an indication something is wrong!
            	sendEvent(name: "panelzone"+result.zone_id, value: "open", displayed: false, isStateChange: true)
                curdevice?.sendEvent(name: "contact", value: "open")
                break

			default:
				log.debug "Unknown zone status received: ${result.zone_name} / ${result.zone_id} is ${result.zone_status}"
				break

		}
	}
    
	if (result.containsKey("output_status"))
	{
		//Got an output status so first try to find the correct child device
		def curdevice = getChildDevices()?.find { it.deviceNetworkId == "alarmchildoutput"+result.output_id}
		//Now switch based on what has happened
		switch (result.output_status)
		{
            case "On":
            	//Output zone has turned on
				sendEvent(name: "paneloutput"+result.output_id, value: "on", displayed: false, isStateChange: true)
				curdevice?.sendEvent(name: "switch", value: "on")
				break

            case "Off":
				//Output zone has turned off
            	sendEvent(name: "paneloutput"+result.output_id, value: "off", displayed: false, isStateChange: true)
                curdevice?.sendEvent(name: "switch", value: "off")
                break

			default:
				log.debug "Unknown output status received: ${result.output_name} / ${result.output_id} is ${result.output_status}"
				break
		}
	}
	
	//This code will pull zone information out of the /getzonenamespage (and allows for zones not in order)
	if (result.containsKey("maxzoneid"))
	{
		log.debug "Handling getzonenames page"
		for (def curzone in result.zones)
		{
			//First setup the prepend and postpend names for the new child zone device
			def thisname = ""
			if (prename != null) {thisname = thisname + prename + " "}
			thisname = thisname + curzone.zonename
			if (postname != null) {thisname = thisname + " " + postname}

			//Now try to find a child device with the right name - wrapped in try in case it fails to find any children
			try
			{
                if (curzone.zonetype == "Output")
                {
            		//This is if you have an output device (not used on Visonic) - First update tile then try to find child device
            		sendEvent(name: "paneloutputname"+(curzone.zoneid), value: curzone.zonename)
					log.debug "Trying to add child with name: ${thisname}, ID: alarmchildoutput${curzone.zoneid} to ${hub.id}"
					def curchildzone = getChildDevices()?.find { it.deviceNetworkId == "alarmchildoutput${curzone.zoneid}"}
                }
                else
                {
            		//This is the normal panel zone name - First update tile then try to find child device
            		sendEvent(name: "panelzonename"+(curzone.zoneid), value: curzone.zonename)
					log.debug "Trying to add child with name: ${thisname}, ID: alarmchildzone${curzone.zoneid} to ${hub.id}"
                	def curchildzone = getChildDevices()?.find { it.deviceNetworkId == "alarmchildzone${curzone.zoneid}"}
                }
			}
			catch (e)
			{
				//Would reach here if it cant find any children or that child doesnt exist so we can try and create it
				log.debug "Couldnt find device, probably doesn't exist so safe to add a new one: ${e}"
			}

			//If we don't have a matching child already, and the name isn't Unknown, then we can finally start creating the child
			if ((curchildzone == null) && (curzone.zonename != "Unknown"))
			{
				try
				{
					switch (curzone.zonetype)
					{
						case ["Magnet", "Contact", "Entry/Exit"]:
							//If it is a magnetic sensor then add it as a contact sensor
							addChildDevice("hubitat", "Virtual Contact Sensor", "alarmchildzone${curzone.zoneid}", [name: "alarm zone test device", isComponent: false])
							log.debug "Creating contact zone"
							break

						case ["Motion"]:
							//If it is a motion sensor then add it as a motion detector
							addChildDevice("hubitat", "Virtual Motion Sensor", "alarmchildzone${curzone.zoneid}", [name: thisname, isComponent: false])
							log.debug "Creating motion zone"
							break

						case ["Shock", "Vibration", "Smoke", "Gas"]:
							//Add the remainders as motion detectors for now - will display motion/no-motion instead of active/inactive sadly
							addChildDevice("hubitat", "Virtual Motion Sensor", "alarmchildzone${curzone.zoneid}", [name: thisname, isComponent: false])
							log.debug "Creating other zone"
							break

						case ["ZWired"]: //Correct this spelling mistake from ZWired to Wired if you are using wired sensors
							//You will also need to change 'Open/Closed Sensor' to 'Motion Detector' if you have a wired motion sensor as the code
                            //below is currently setup for a wired contact sensor
							addChildDevice("hubitat", "Virtual Contact Sensor", "alarmchildzone${curzone.zoneid}", [name: thisname, isComponent: false])
							log.debug "Creating motion zone"
							break

						case ["Output"]:
							//This is an output zone for controlling on/off
							addChildDevice("hubitat", "Virtual Switch", "alarmchildoutput${curzone.zoneid}", [name: "${thisname} Output", isComponent: false])
							log.debug "Creating output"
							break

						default:
							log.debug "Unknown sensor found, we'll have to ignore for now"
							break
					}
				}
				catch (e)
				{
					log.error "Couldnt add device, probably already exists: ${e}"
				}
			}
		}
	}
}

private getAction(uri)
{ 
	log.debug "uri ${uri}"
	updateDNI()

	def userpass

	if(password != null && password != "") 
		userpass = encodeCredentials("admin", password)
	
	def headers = getHeader(userpass)
  
	def hubAction = new hubitat.device.HubAction(
		method: "GET",
		path: uri,
		headers: headers
		)
	return hubAction	
}

def parseDescriptionAsMap(description)
{
	description.split(",").inject([:]) { map, param ->
		def nameAndValue = param.split(":")
		map += [(nameAndValue[0].trim()):nameAndValue[1].trim()]
	}
}

private getHeader(userpass = null)
{
	def headers = [:]
	headers.put("Host", getHostAddress())
	headers.put("Content-Type", "application/x-www-form-urlencoded")
	if (userpass != null)
	   headers.put("Authorization", userpass)
	return headers
}

private encodeCredentials(username, password)
{
	def userpassascii = "${username}:${password}"
	def userpass = "Basic " + userpassascii.toString()
	return userpass
}

private updateDNI()
{ 
	if (state.dni != null && state.dni != "" && device.deviceNetworkId != state.dni)
	{
	   device.deviceNetworkId = state.dni
	}
}

private getHostAddress()
{
	if(getDeviceDataByName("ip") && getDeviceDataByName("port"))
	{
		return "${getDeviceDataByName("ip")}:${getDeviceDataByName("port")}"
	}
	else
	{
		return "${ip}:80"
	}
}


def outputon(outputnumber, period = 0)
{
	//Turn an output on (for a period of time - default to 0 (permanent) if not specified)
	log.debug "outputon()"
	getAction("/on?output=${outputnumber}&period=${period}")
}
def outputoff(outputnumber, period = 0)
{ 
	//Turn an output off (no period allowed for off so ignored)
	log.debug "outputoff()"
	getAction("/off?output=${outputnumber}")
}

def childRefresh(String dni) {
    log.debug "childRefresh($dni)"
    //Dont do anything yet as we dont have a local state to refresh with
}
def childOn(String dni) {
    log.debug "childOn($dni)"
    def outputnumber = dni.replaceAll("alarmchildoutput", "")
    outputon(outputnumber, 0)
}
def childOff(String dni) {
    log.debug "childOff($dni)"
    def outputnumber = dni.replaceAll("alarmchildoutput", "")
    outputoff(outputnumber, 0)
}

def on1() { outputon(1, 0) }
def on2() { outputon(2, 0) }
def on3() { outputon(3, 0) }
def on4() { outputon(4, 0) }
def on5() { outputon(5, 0) }
def on6() { outputon(6, 0) }
def on7() { outputon(7, 0) }
def on8() { outputon(8, 0) }

def off1() { outputoff(1, 0) }
def off2() { outputoff(2, 0) }
def off3() { outputoff(3, 0) }
def off4() { outputoff(4, 0) }
def off5() { outputoff(5, 0) }
def off6() { outputoff(6, 0) }
def off7() { outputoff(7, 0) }
def off8() { outputoff(8, 0) }
