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
 *  Neo Thermostat (Child Device of Neo Hub Bridge)
 *
 *  Author: Chris Charles (cjcharles0)
 *  Date: 2017-04-26
 *  
 *  Important note, since each command is requesting something from the server, they cannot be void's otherwise
 *  nothing seems to come back from ST, even though it should receive the JSON anyway
 */

import groovy.json.JsonSlurper

metadata {
	definition (name: "Neo Thermostat", namespace: "cjcharles0", author: "Chris Charles")
    { 
    	capability "Refresh"
		capability "Polling"
        capability "Configuration"
        
        capability "Sensor"
		capability "Temperature Measurement"
		capability "Thermostat"
        
        capability "Actuator"
        capability "Relay Switch"
        capability "Switch"
		
        command "refreshinfo" //This is the actual command to update the thermostat from parent
        command "refresh" //This is used for the overall refresh process
        
        command "boostOneHour" // Custom
		command "boostHours" // Custom
		command "boostTempHours" // Custom
        
        command "setHeatingSetpoint" //Required by ST for thermostat type
        command "setCoolingSetpoint" //Required by ST for thermostat type
        command "setThermostatSetpoint" //Required by ST for thermostat type
        command "setTemperature" //Required by ST for thermostat type
        command "heat" //Required by ST for thermostat type
        command "emergencyHeat" //Required by ST for thermostat type
        command "cool" //Required by ST for thermostat type
        command "setThermostatMode" //Required by ST for thermostat type
        command "fanOn" //Required by ST for thermostat type
        command "fanAuto" //Required by ST for thermostat type
        command "fanCirculate" //Required by ST for thermostat type
        command "setThermostatFanMode" //Required by ST for thermostat type
        command "auto" //Required by ST for thermostat type
        
        command "off" //Required by ST for thermostat type
        command "on" //Required by ST for switch type
        
        command "ensureAlexaCapableMode"
        command "raiseSetpoint" // Custom
		command "lowerSetpoint" // Custom
		command "increaseDuration" // Custom
		command "decreaseDuration" // Custom
		command "setTempHoldOn" // Custom
		command "setTempHoldOff" // Custom
		command "setTimerOn" // Custom
		command "setTimerOff" // Custom
        command "away" // Custom
		command "awayOff" // Custom
		command "holidayOff" // Custom

        attribute "holdtime","string" // Custom for how long to hold for
		attribute "nextSetpointText", "string" // Custom for text display
		attribute "statusText", "string" // Custom for neohub response
        
  		attribute "temperature","number" // Temperature Measurement
		attribute "thermostatSetpoint","number" // Thermostat setpoint      
        attribute "coolingSetpoint","number" // Thermostat setpoint
		attribute "heatingSetpoint","number" // Thermostat setpoint
        
	}
    
    preferences {
        section {
        	//Preferences here
        }
	}

//Thermostat Temp and State
	tiles(scale: 2) {

		// Main multi information tile
		multiAttributeTile(name:"temperatureDisplay", type:"thermostat", width:6, height:3, canChangeIcon: true) { //, canChangeBackground: true
			tileAttribute("device.temperature", key: "PRIMARY_CONTROL") {
				attributeState("default", label:'${currentValue}', unit:"dC", icon: 'st.Weather.weather2',
					backgroundColors:[
							// Celsius
							[value: 0, color: "#153591"],
							[value: 9, color: "#1e9cbb"],
							[value: 15, color: "#90d2a7"],
							[value: 22, color: "#44b621"],
							[value: 28, color: "#f1d801"],
							[value: 32, color: "#d04e00"],
							[value: 36, color: "#bc2323"]
					] )
			}
			// Operating State - used to get background colour when type is 'thermostat'.
            tileAttribute("device.thermostatOperatingState", key: "OPERATING_STATE") {
                attributeState("heating", backgroundColor:"#e86d13", defaultState: true)
                attributeState("idle", backgroundColor:"#00A0DC")
                attributeState("cooling", backgroundColor:"#00A0DC")
            }
            tileAttribute("device.thermostatMode", key: "THERMOSTAT_MODE") {
                attributeState("auto", label:' ')
                attributeState("heat", label:' ', defaultState: true)
                attributeState("off", label:' ')
                attributeState("cool", label:' ')
            }
            tileAttribute("device.heatingSetpoint", key: "HEATING_SETPOINT") {
                attributeState("heatingSetpoint", label:'${currentValue}', unit:"dC", defaultState: true)
            }
            tileAttribute("device.coolingSetpoint", key: "COOLING_SETPOINT") {
                attributeState("coolingSetpoint", label:'${currentValue}', unit:"dC", defaultState: true)
            }
		}
       
		valueTile("manualSetpoint", "device.manualSetpoint", width: 1, height: 1, decoration: "flat") {
			state "default", label:'${currentValue}', unit:"dC", backgroundColor:"#ffffff"
		}
		standardTile("raisethermostatSetpoint", "device.raisethermostatSetpoint", width: 1, height: 1, decoration: "flat") {
			state "default", action:"raiseSetpoint", label:'${currentValue}'//, icon:"st.thermostat.thermostat-up"
		}
		standardTile("lowerthermostatSetpoint", "device.lowerthermostatSetpoint", width: 1, height: 1, decoration: "flat") {
			state "default", action:"lowerSetpoint", label:'${currentValue}'//, icon:"st.thermostat.thermostat-down"
		}
        
		standardTile("increaseTime", "device.increaseTime", width: 1, height: 1, decoration: "flat") {
			state "default", action:"increaseDuration", label:'+30m'
		}
        standardTile("decreaseTime", "device.decreaseTime", width: 1, height: 1, decoration: "flat") {
			state "default", action:"decreaseDuration", label:'-30m'
		}
		valueTile("holdtime", "device.holdtime", inactiveLabel: false, decoration: "flat", width: 1, height: 1) {
			state "default", label:'${currentValue}' // icon TBC
		}
        
        standardTile("setTempHold", "setTempHold", inactiveLabel: false, decoration: "flat", width: 2, height: 1) {
			state "setTemp", action:"setTempHoldOn", label:'Set Temp' // icon TBC
			state "setHold", action:"setTempHoldOn", label:'Set Hold' // icon TBC
            state "tempWasSet", action:"setTempHoldOn", label:'Temp Was Set' // icon TBC
            state "cancelHold", action:"setTempHoldOff", label:'Cancel Hold' // icon TBC
		}
        
        
		valueTile("nextSetpointText", "device.nextSetpointText", width: 3, height: 1) {
			state "default", label:'${currentValue}'
		}
        valueTile("statusText", "statusText", decoration: "flat", width: 3, height: 1) {     //, inactiveLabel: false
			state "default", label:'${currentValue}'
		}
        
        
        valueTile("floortemp", "floortemp", decoration: "flat", width: 1, height: 1) {
    		state "floortemp", label:'Floor Temp\r\n${currentValue}'
		}
        standardTile("awayholiday", "awayholiday", inactiveLabel: false, decoration: "flat", width: 2, height: 1) {
			state "off", action:"away", label:'Set Away / Standby' // icon TBC
			state "away", action:"awayOff", label:'Away Activated\r\nPress to cancel' // icon TBC
			state "holiday", action:"holidayOff", label:'Holiday - Press\r\nto cancel all' // icon TBC
		}
		standardTile("refresh", "device.refresh", inactiveLabel: false, decoration: "flat", width: 1, height: 1) {
			state "default", action:"refresh", label: "Refresh", icon:"st.secondary.refresh"
		}
		
		main "temperatureDisplay"
		details(
				[
				"temperatureDisplay",
				"lowerthermostatSetpoint", "manualSetpoint", "raisethermostatSetpoint",
                "decreaseTime", "holdtime", "increaseTime", 
				"nextSetpointText", "setTempHold", "refresh", 
                "awayholiday", "floortemp", "statusText"])
	}
}


def refresh() {
    //Default refresh method which calls immediate update and ensures it is scheduled
	log.debug "Refreshing thermostat data from parent"
	refreshinfo()
    runEvery5Minutes(refreshinfo)
}
def refreshinfo() {
	//Actually get a refresh from the parent/Neohub
    parent.childRequestingRefresh(device.deviceNetworkId)
    if (parent.childGetDebugState()) {
    	state.debug = true
    } else {
    	state.debug = false
    }
}

def ping() {
    log.debug "ping()"
    refresh()
}
def poll() {
    log.debug "poll()"
    refresh()
}
def installed() {
	//Here we have a new device so lets ensure that all the tiles are correctly filled with something
    log.debug "installed()"
	updated()
    ensureAlexaCapableMode() //Shouuldnt be needed but done just in case
}
def updated() {
	//On every update, lets reset hold time and holding temperature
    log.debug "updated()"
    
    //Rremove any existing schedules since 
    unschedule()
    
    def cmds = []
    cmds << sendEvent(name: "statusText", value: "Please wait for next refresh or press refresh now")
    
    cmds << sendEvent(name: "holdtime", value: "0:00")
    
    cmds << sendEvent(name: "manualSetpoint", value: "18°C")
    cmds << sendEvent(name: "raisethermostatSetpoint", value: "+1°C")
    cmds << sendEvent(name: "lowerthermostatSetpoint", value: "-1°C")
    return cmds
}


def processNeoResponse(result) {
	//Response received from Neo Hub so process it (mainly used for refresh, but could also process the success/fail messages)
	def statusTextmsg = ""
	def cmds = []

	//log.debug result
    if (result.containsKey("relaydevice")) {
    	//If we have a relaydevice key then we have a response to a command we sent, so process it here
        if (result.relayresult.containsKey("result")) {
    		//We have a success result from a command so process it here by pasting in the response and updating tile
            log.debug "success on last command: " + result.relayresult.result
            //Would love to refresh information at this point, but it will fail as the Neostats take a while to update
            //refreshinfo()
        	cmds << sendEvent(name: "statusText", value: result.relayresult)
        	cmds << sendEvent(name: "nextSetpointText", value: "Waiting for next refresh", displayed: false)
        }
    }
    else if (result.containsKey("device") && result.containsKey("STANDBY")) {
    	//If we have a device key and STANDBY key then it is probably a refresh command
        
        //First store the update/refresh date/time
        def dateTime = new Date()
        def updateddatetime = dateTime.format("yyyy-MM-dd HH:mm", location.timeZone)
        cmds << sendEvent(name: "statusText", value: "Last refreshed info at ${updateddatetime}", displayed: false, isStateChange: true)
        
        //Now process information shared between all devices including standby/away and Floor Temp
        if (result.containsKey("STANDBY") && result.containsKey("HOLIDAY")) {
        	//Update standby/away status
            if (result.STANDBY == false && result.HOLIDAY == false) {
                cmds << sendEvent(name: "awayholiday", value: "off", displayed: true)
            }
            else if (result.STANDBY == true) {
                cmds << sendEvent(name: "awayholiday", value: "away", displayed: true)
            }
            else if (result.HOLIDAY == true) {
                cmds << sendEvent(name: "awayholiday", value: "holiday", displayed: true)
            }
        }
        if (result.containsKey("CURRENT_FLOOR_TEMPERATURE")) {
        	//Update the floor temperature in case anybody cares!
            def flrtempstring
            if (result.CURRENT_FLOOR_TEMPERATURE >= 127) {
            	flrtempstring = "N/A"
            }
            else {
            	flrtempstring = result.CURRENT_FLOOR_TEMPERATURE
            }
        	cmds << sendEvent(name: "floortemp", value: flrtempstring, displayed: false)
        }

        if (result.containsKey("STAT_MODE")) {
        	//This is used to identify what type of device it is
			if (result.STAT_MODE.containsKey("TIMECLOCK")) {
                if (result.STAT_MODE.TIMECLOCK == true) {
                	//This device is a timer so lets update it (starting by setting it to On/Off mode)
                    cmds << sendEvent(name: "raisethermostatSetpoint", value: "On", displayed: false)
                    cmds << sendEvent(name: "lowerthermostatSetpoint", value: "Off", displayed: false)
                    if (result.DEVICE_TYPE == 7) {
                    	//Device is a Timeclock
                        cmds << sendEvent(name: "manualSetpoint", value: "Timer Boost 1h", displayed: false)
                    }
                    else if (result.DEVICE_TYPE == 6) {
                    	//Device is a Neoplug
                        cmds << sendEvent(name: "manualSetpoint", value: "Neoplug Control", displayed: false)
                    }

                    if (result.containsKey("HOLD_TIME") && result.containsKey("HOLD_TEMPERATURE")) {
                    	//Update the set temp text or holding time
                        def onoffstring
                        if (result.containsKey("TIMER")) {
                        	//Update the on/off string here, but not needed as shouldnt be relevant
                            if (result.TIMER) {
                            	onoffstring = "ON"
                                cmds << sendEvent(name: "thermostatOperatingState", value: "heating", displayed: true)
                            }
                            else {
                            	 onoffstring = "OFF"
                                 cmds << sendEvent(name: "thermostatOperatingState", value: "idle", displayed: true)
                            }
                        }
						cmds << sendEvent(name: "temperature", value: onoffstring, displayed: true)
                        cmds << sendEvent(name: "thermostatSetpoint", value: onoffstring, displayed: false)
                        cmds << sendEvent(name: "heatingSetpoint", value: onoffstring, displayed: false)
                        cmds << sendEvent(name: "coolingSetpoint", value: onoffstring, displayed: false)
                        if (result.HOLD_TIME == "0:00") {
                            //Here we have zero hold time so run until next on time
                            statusTextmsg = "Running to schedule or manual mode"
                            //Now send the update
                            cmds << sendEvent(name: "nextSetpointText", value: statusTextmsg)
                            //Lastly if we are here then there should be no holds in place - hence ensure the button doesn't say cancel hold
                            chooseSetTempOrSetHold()
                        }
                        else {
                            //Here we do have a hold time so display hold info
                            statusTextmsg = "Holding " + onoffstring + " for " + result.HOLD_TIME
                            cmds << sendEvent(name: "nextSetpointText", value: statusTextmsg, displayed: false)
        					cmds << sendEvent(name: "setTempHold", value: "cancelHold", displayed: false)
                        }
                    }
                }
                else {
                	//Here we should check that the +/- temp buttons are set to +/- rather than on/off (sometimes heatmiser messes this up)
                    if (device.currentValue("raisethermostatSetpoint") == "+1°C") {
                        cmds << sendEvent(name: "raisethermostatSetpoint", value: "+1°C", displayed: false)
                        cmds << sendEvent(name: "lowerthermostatSetpoint", value: "-1°C", displayed: false)
                    }
                }
			}
			if (result.STAT_MODE.containsKey("THERMOSTAT")) {
                if (result.STAT_MODE.THERMOSTAT == true) {
                	//This device is a thermostat so lets update it!
					if (result.containsKey("HEATING")) {
                        //Update the tiles to show that it is currently calling for heat from the boiler
                        if (result.HEATING) {
                            cmds << sendEvent(name: "thermostatOperatingState", value: "heating", displayed: true)
                        } else {
                            cmds << sendEvent(name: "thermostatOperatingState", value: "idle", displayed: true)
                        }
                    }
                    if (result.containsKey("HOLD_TIME") && result.containsKey("HOLD_TEMPERATURE") && result.containsKey("NEXT_ON_TIME")) {
                    	//Update the set temp text or holding time
                        if (result.HOLD_TIME == "0:00") {
                            //Here we have zero hold time so run until next on time
                            statusTextmsg = "Set to " + result.CURRENT_SET_TEMPERATURE + "C until "
                            if (result.NEXT_ON_TIME.reverse().take(3).reverse() == "255") {
                                //If we see 255:255 in hh:mm field then it is set permanently (hence just check the last three digits)
                                statusTextmsg = statusTextmsg + "changed"
                            }
                            else {
                                //Otherwise add on the time for next change
                                statusTextmsg = statusTextmsg + result.NEXT_ON_TIME.reverse().take(5).reverse()
                            }
                            if (result.containsKey("HOLIDAY") && result.containsKey("STANDBY")) {
                            	//If we have a holiday flag set to true, and a standby flag set to false then display remaining holiday (rounded down)
                            	if ((result.HOLIDAY == true) && (result.STANDBY == false)) {
                            		statusTextmsg = statusTextmsg + "\r\nHoliday for " + result.HOLIDAY_DAYS + " more days"
                                }
                            }
                            //Now send the update
                            cmds << sendEvent(name: "nextSetpointText", value: statusTextmsg)
                            //Lastly if we are here then there should be no holds in place - hence ensure the button doesn't say cancel hold
                            chooseSetTempOrSetHold()
                        }
                        else {
                            //Here we do have a hold time so display temp and time
                            statusTextmsg = "Holding " + result.HOLD_TEMPERATURE + "°C for " + result.HOLD_TIME
                            cmds << sendEvent(name: "nextSetpointText", value: statusTextmsg, displayed: false)
        					cmds << sendEvent(name: "setTempHold", value: "cancelHold", displayed: false)
                        }
                    }
                    if (result.containsKey("CURRENT_TEMPERATURE") && result.containsKey("CURRENT_SET_TEMPERATURE")) {
                        //Got a temperature so update the tile
                        log.debug "Temperature is: " + result.CURRENT_TEMPERATURE + " - Setpoint is: " + result.CURRENT_SET_TEMPERATURE + " - Calling for heat? " + result.HEATING
                        cmds << sendEvent(name: "temperature", value: result.CURRENT_TEMPERATURE, displayed: true)

                        //Got a set temperature so update it if above 6 (legacy from old firmware setting 5 for away mode, but keeping it for now)
                        def settempint = result.CURRENT_SET_TEMPERATURE.toBigDecimal().toInteger() + 0 //.toBigDecimal().toInteger()
                        if (settempint >= 6) {
                            cmds << sendEvent(name: "thermostatSetpoint", value: settempint, displayed: false)
                            cmds << sendEvent(name: "heatingSetpoint", value: settempint, displayed: false)
                            cmds << sendEvent(name: "coolingSetpoint", value: settempint, displayed: false)
                        }
                    }
                }
			}
        }
        

	}
    return cmds
}


def increaseDuration() {
	def cmds = []
    def durationMins
    durationMins = timeStringToMins(device.currentValue("holdtime"))
    if (durationMins == 300) {
    	durationMins = 0
    }
    else {
    	durationMins = durationMins + 30
    }
    chooseSetTempOrSetHold(durationMins)
    cmds << sendEvent(name: "holdtime", value: "${minsToTimeString(durationMins)}", displayed: false, isStateChange: true)
    return cmds
}

def decreaseDuration() {
	def cmds = []
    def durationMins
    durationMins = timeStringToMins(device.currentValue("holdtime"))
    if (durationMins == 0) {
    	durationMins = 300
    }
    else {
    	durationMins = durationMins - 30
    }
    chooseSetTempOrSetHold(durationMins)
    cmds << sendEvent(name: "holdtime", value: "${minsToTimeString(durationMins)}", displayed: false, isStateChange: true)
    return cmds
}

def raiseSetpoint() {
	//Called by tile to increase set temp box
	def cmds = []
    if (device.currentValue("raisethermostatSetpoint") == "On") {
    	//The thermostat is a timer/plug so use timer on rather than changing temp
        setTimerOn()
    }
    else {
		def newtemp = device.currentValue("manualSetpoint").replaceAll("°C", "").toInteger() + 1
    	cmds << sendEvent(name: "manualSetpoint", value: "${newtemp}°C")
		chooseSetTempOrSetHold()
    }
    return cmds
}
def lowerSetpoint() {
    //Called by tile to decrease set temp box
	def cmds = []
    if (device.currentValue("raisethermostatSetpoint") == "On") {
    	//The thermostat is a timer/plug so use timer off rather than changing temp
        setTimerOff()
    }
    else {
		def newtemp = device.currentValue("manualSetpoint").replaceAll("°C", "").toInteger() - 1
    	cmds << sendEvent(name: "manualSetpoint", value: "${newtemp}°C")
		chooseSetTempOrSetHold()
    }
    return cmds
}

def setTimerOn() {
	//We already know it is a timer/plug, so lets do something based on that
	if (device.currentValue("manualSetpoint") == "Neoplug Control") {
    	//The thermostat is a plug turn it on
        parent.childTimerOn(device.deviceNetworkId)
    }
    else {
    	//Set it to boost for an hour as that is all we can do
        parent.childTimerHoldOn("60", device.deviceNetworkId)
    }
}
def setTimerOff() {
	//We already know it is a timer/plug, so lets do something based on that
	if (device.currentValue("manualSetpoint") == "Neoplug Control") {
    	//The thermostat is a plug turn it on
		parent.childTimerOff(device.deviceNetworkId)
    }
    else {
    	//Set it to boost for an hour as that is all we can do
        parent.childTimerHoldOff("60", device.deviceNetworkId)
    }
}
def on() {
	setTimerOn()
}
def off() {
	setTimerOff()
}

def away() {
	//Set away mode on
	def cmds = []
	if (state.debug) log.debug "${device.label}: away()"
	parent.childFrostOn(device.deviceNetworkId)
    cmds << sendEvent(name: "awayholiday", value: "away", displayed: true)
    return cmds
}
def awayOff() {
	//Set away mode off
	def cmds = []
	if (state.debug) log.debug "${device.label}: awayOff()"
	parent.childFrostOff(device.deviceNetworkId)
    cmds << sendEvent(name: "awayholiday", value: "off", displayed: true)
    return cmds
}
def holidayOff() {
	//Cancel holiday mode
	def cmds = []
	if (state.debug) log.debug "${device.label}: holidayOff()"
	parent.childCancelHoliday(device.deviceNetworkId)
    cmds << sendEvent(name: "awayholiday", value: "off", displayed: true)
    return cmds
}

def chooseSetTempOrSetHold(mins = null) {
	def cmds = []
	if (mins == null) { mins = timeStringToMins(device.currentValue("holdtime")) }
	if (mins == 0) {
    	//We have 0:00 as hh:mm, so somebody would only be able to set temp
        cmds << sendEvent(name: "setTempHold", value: "setTemp", displayed: false)
    }
    else {
    	//Otherwise they can set a hold
    	cmds << sendEvent(name: "setTempHold", value: "setHold", displayed: false)
    }
    return cmds
}

def boostOneHour() {
	//Get current setpoint and add two degrees to ensure it turns on the heating
	def desiredTemp = device.currentValue("thermostatSetpoint").toInteger() + 2
    
    //Send the command to the Neohub
	parent.childHold(desiredTemp.toString(), "1", "0", device.deviceNetworkId)
}

def boostHours(durationHours = null) {
	//Get current setpoint and add two degrees to ensure it turns on the heating
	def desiredTemp = device.currentValue("thermostatSetpoint").toInteger() + 2
    
    //Also target 1 hour if not specified
    if (desiredHours==null) desiredHours = 1
    
    //Finally send the command to the Neohub
	parent.childHold(desiredTemp.toString(), desiredHours.toString(), "0", device.deviceNetworkId)
}

def boostTempHours(desiredTemp = null, desiredHours = null) {
	//First get current setpoint in case we need to use it (target +2C if not specified)
	def currentsetpoint = device.currentValue("thermostatSetpoint").toInteger()
    if (desiredTemp==null) desiredTemp = currentsetpoint + 2
    //Also target 1 hour if not specified
    if (desiredHours==null) desiredHours = 1
    
    //Finally send the command to the Neohub
	parent.childHold(desiredTemp.toString(), desiredHours.toString(), "0", device.deviceNetworkId)
}

def setTempHoldOn() {
	def cmds = []
    def newtemp = device.currentValue("manualSetpoint").replaceAll("°C", "")
    def isthermostat = (device.currentValue("raisethermostatSetpoint") == "On") ? false : true
    
    if (state.debug) log.debug "${device.label}: Set temp hold to ${newtemp} for ${device.currentValue("holdtime")} - setTempHoldOn()"

    if (device.currentValue("holdtime") == "0:00") {
    	//Hold time is zero, so use set temp or schedule override
		cmds << sendEvent(name: "setTempHold", value: "tempWasSet", displayed: false, isStateChange: true)
        if (isthermostat) {
            //The device is a normal thermostat so use normal set temp
            parent.childSetTemp(newtemp.toString(), device.deviceNetworkId)
        }
        else {
            //The device is a timer so use timer on for 0:00 (same as pressing on button)
            setTimerOn()
        }
    }
    else {
    	//Hold time is above zero, so hold for the chosen time (either timer or thermostat)
        if (state.debug) log.debug hours + "hours, " + minutes + "mins of hold time"
        cmds << sendEvent(name: "setTempHold", value: "cancelHold", displayed: false, isStateChange: true)
        if (isthermostat) {
            //The device is a normal thermostat so use normal hold
            def hoursandmins = timeStringToHoursMins(device.currentValue("holdtime"))
            parent.childHold(newtemp.toString(), hoursandmins[0], hoursandmins[1], device.deviceNetworkId)
        }
        else {
            //The device is a timer so use timer on but first convert h:mm into mmm
            def minutes = timeStringToMins(device.currentValue("holdtime")).toString()
            parent.childTimerHoldOn(minutes, device.deviceNetworkId)
        }
	}
    //Also update the tile immediately (shouldn't really do this as should wait until the next update,
    //but if we dont do it now, then Alexa and Google Home get really confused)
    if (isthermostat) cmds << sendEvent(name: "thermostatSetpoint", value: newtemp, displayed: false)
	if (isthermostat) cmds << sendEvent(name: "heatingSetpoint", value: newtemp, displayed: false)
	if (isthermostat) cmds << sendEvent(name: "coolingSetpoint", value: newtemp, displayed: false)
    return cmds
}

def setTempHoldOff() {
	//Cancel the temp hold
	def cmds = []
	if (state.debug) log.debug "${device.label}: cancel hold/temp - setTempHoldOff()"
    if (device.currentValue("raisethermostatSetpoint") == "On") {
    	//The device is a timer so use timer hold with 0 mins to turn off hold
        parent.childTimerHoldOn("0", device.deviceNetworkId)
    }
    else {
    	//The device is a normal thermostat so use cancel hold
		parent.childCancelHold(device.deviceNetworkId)
    }
	chooseSetTempOrSetHold()
    return cmds
}


//These commands are used by Alexa
def setHeatingSetpoint(number) {
	def cmds = []
	cmds << sendEvent(name: "thermostatSetpoint", value: number, displayed: false)
	cmds << sendEvent(name: "heatingSetpoint", value: number, displayed: false)
	cmds << sendEvent(name: "coolingSetpoint", value: number, displayed: false)
	parent.childSetTemp(number.toString(), device.deviceNetworkId)
    return cmds
}
def setThermostatSetpoint(number) {
	def cmds = []
	cmds << sendEvent(name: "thermostatSetpoint", value: number, displayed: false)
	cmds << sendEvent(name: "heatingSetpoint", value: number, displayed: false)
	cmds << sendEvent(name: "coolingSetpoint", value: number, displayed: false)
	parent.childSetTemp(number.toString(), device.deviceNetworkId)
    return cmds
}
def setTemperature(number) {
	def cmds = []
	cmds << sendEvent(name: "thermostatSetpoint", value: number, displayed: false)
	cmds << sendEvent(name: "heatingSetpoint", value: number, displayed: false)
	cmds << sendEvent(name: "coolingSetpoint", value: number, displayed: false)
	parent.childSetTemp(number.toString(), device.deviceNetworkId)
    return cmds
}

private timeStringToMins(String timeString){
	if (timeString?.contains(':')) {
    	def hoursandmins = timeString.split(":")
        def mins = hoursandmins[0].toInteger() * 60 + hoursandmins[1].toInteger()
        if (state.debug) log.debug "${timeString} converted to ${mins}" 
        return mins
    }
}

private minsToTimeString(Integer intMins) {
	def timeString =  "${(intMins/60).toInteger()}:${(intMins%60).toString().padLeft(2, "0")}"
    if (state.debug) log.debug "${intMins} converted to ${timeString}"
    return timeString
}

private timeStringToHoursMins(String timeString){
	if (timeString?.contains(':')) {
    	def hoursMins = timeString.split(":")
        if (state.debug) log.debug "${timeString} converted to ${hoursMins[0]}:${hoursMins[1]}"
        return hoursMins
    }
}

private minsToHoursMins(Integer intMins) {
	def hoursMins = []
    hoursMins << (intMins/60).toInteger()
    hoursMins << (intMins%60).toInteger()
    if (state.debug) log.debug "${intMins} converted to ${hoursMins[0]}:${hoursMins[1]}" 
    return hoursMins
}

//Dont use any of these yet as I havent worked out why they would be needed! 
//Just log that they were triggered for troubleshooting
def heat() {
	log.debug "heat()"
}
def emergencyHeat() {
	log.debug "emergencyHeat()"
}
def setThermostatMode(chosenmode) {
	log.debug "setThermostatMode() - ${chosenmode}"
}
def fanOn() {
	log.debug "fanOn()"
}
def fanAuto() {
	log.debug "fanAuto()"
}
def fanCirculate() {
	log.debug "fanCirculate()"
}
def setThermostatFanMode(chosenmode) {
	log.debug "setThermostatFanMode() - ${chosenmode}"
}
def cool() {
	log.debug "cool()"
}
def setCoolingSetpoint(number) {
	log.debug "setCoolingSetpoint() - ${number}"
}
def auto() {
	log.debug "auto()"
}

//This shouldnt be needed any more
def ensureAlexaCapableMode() {
	sendEvent(name: "thermostatMode", value: "heat")
}
