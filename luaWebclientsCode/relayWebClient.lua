 function QuickApp:relayControl()
 --self:debug("relayControl")
 local address = self.ip.."/RELAY"
  self.http:request(address, {
 options={
 headers = self.controlHeaders,
 data = requestBody,
 method = 'POST',
 timeout = 1200
 },
 success = function(status)
 local result = json.decode(status.data)
 if result[1] then
 if result[1].error then
 self:debug ("ERROR")
 self:debug (" type: " .. result[1].error.type)
 self:debug (" address: " .. result[1].error.address)
 self:debug (" description: " .. result[1].error.description)
 elseif result[1].success then
 self:updateProperty("userName", result[1].success.username)
 else
 self:debug ("unknown response structure: ")
 self:debug(status)
 end
 else
 self:debug ("unknown response structure: ")
 self:debug(status)
 end
 end,
 error = function(error)
 self:debug "ERROR"
 self:debug(error)
 end
 })
--self:loop(1)
 end

 function QuickApp:turnOn()
  --self:debug("turnOn")
   self:relayControl()
   -- self:updateProperty("value", true)   
end
function QuickApp:turnOff()
 --self:debug("turnOff")
self:relayControl()
  --self:updateProperty("value", false)
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
    --self:debug("jsonTMP:", jsonTMP)
            --this does not work
             --data = json.decode(response.data)   
             --self:debug(" data:", data)
              --self:debug(" data.contents:",data.contents)
          list=string.split(jsonTMP,",")
          listValue = string.split(list[1],":")
          listName = string.split(list[2],":")
          
            --self:debug(" json contents:", string.sub(listValue[1], 2, -2))
            if listValue[1] and listValue[2] then
           
                if (string.sub(listValue[1], 2, -2)  == "door") then
     
                 if listValue[2] =="\"1\"" the
                      self:updateProperty("value", true)
                    else
                        self:updateProperty("value", false)
                 end
                end

                --self:updateProperty(listValue[1],listValue[2]) 
                 
                 if listName[1] and listName[2] then
                 --self:debug(" json contents:", string.sub(listName[2], 2, -2))
                --   self:updateProperty("name",string.sub(listName[2], 2, -2))
                end
            end
        end,
        error = function(error)            self:debug('error: ' .. json.encode(error))
        end
    }) 
       fibaro.setTimeout(10000, function() 
        self:loop(text+1)
    end)
end



 
function QuickApp:onInit()
 --   ip = ""
    --self:debug("onInit")
    self.http = net.HTTPClient() 
    self.ip="some web page here" 
--    self.baseUrl = "http://".. self:getVariable("ip")
    self:loop(1);
    --self:debug("onInit out")
end