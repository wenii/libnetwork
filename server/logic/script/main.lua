
-- 接收到新的连接
function onAccept(connID)
    print("onAccept:".. connID)
end

-- 客户端断开连接
function onDisconnect(connID)
    print("onDisconnect:".. connID)
end

-- 逻辑循环
function update(dt)
    --print("update..."..dt)
end

-- 路由逻辑
function onRouter(getID, clientID, data, size)
    print("getID:"..getID.." clientID:"..clientID.." data:"..data.." size:"..size)
end

