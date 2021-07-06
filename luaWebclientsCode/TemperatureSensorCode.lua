-- Multilevel sensor type have no actions to handle
-- To update multilevel sensor state, update property "value" with integer
-- Eg. self:updateProperty("value", 37.21) 

-- To set unit of the sensor, update property "unit". You can set it on QuickApp initialization
-- Eg. 
-- function QuickApp:onInit()
--     self:updateProperty("unit", "KB")
-- end

-- To update controls you can use method self:updateView(<component ID>, <component property>, <desired value>). Eg:  
-- self:updateView("slider", "value", "55") 
-- self:updateView("button1", "text", "MUTE") 
-- self:updateView("label", "text", "TURNED ON") 

-- This is QuickApp inital method. It is called right after your QuickApp starts (after each save or on gateway startup). 
-- Here you can set some default values, setup http connection or get QuickApp variables.
-- To learn more, please visit: 
--    * https://manuals.fibaro.com/home-center-3/
--    * https://manuals.fibaro.com/home-center-3-quick-apps/
function QuickApp:updateLabel(list,labelText,labelID)

if ( labelText == "temperature" ) then
  self:updateProperty("value",tonumber(string.sub(list[2], 2, -2)))
  measureUnits = " C"
 elseif ( labelText == "humidity") then
  measureUnits = " %"
 else
  measureUnits = ""
 end

 if list[1] and list[2] then
   if (string.sub(list[1], 2, -2)  == labelText) then
   --self:debug(list[2])
      self:updateView(labelID,"text",labelText..": "..string.sub(list[2], 2, -2)..measureUnits)
   end
 end
end

function QuickApp:loop(text)
     --self:debug("Text from loop:" , text)
    local address = self.ip.."/json"
        self.http:request(address, {
        options={
            headers = { 
                Accept = "application/json"
            },
            checkCertificate = false,
            method = 'GET'
        },
        success = function(response)
            --self:debug("response status:", response.status) 
            --self:debug("headers:", response.headers["Content-Type"]) 
            --self:debug("response data:", response.data)
            
jsonTMP = string.sub(response.data, 2, -2) 
self:debug("jsonTMP:", jsonTMP)
            --this does not work
             --data = json.decode(response.data)   
             --self:debug(" data:", data)
            --self:debug(" data.contents:",data.contents)
          list=string.split(jsonTMP,",")
          listTemp = string.split(list[1],":")
          listHum = string.split(list[2],":")
          listHi = string.split(list[3],":")
          listName = string.split(list[4],":")
          
          self:updateLabel(listTemp,"temperature","temperatureID")
          self:updateLabel(listHum,"humidity","humidityID")            
        end,
        error = function(error)            self:debug('error: ' .. json.encode(error))
        end
    }) 
       fibaro.setTimeout(5*60*1000, function() 
        self:loop(text+1)
    end)
end

function QuickApp:onInit()
    self.http = net.HTTPClient() 
    self.ip= "your ip here" 
    self:loop(1);
end