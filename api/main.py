# signaler.py
import asyncio
from fastapi import FastAPI, WebSocket, WebSocketDisconnect

app = FastAPI()

# Stores connections by room_id
rooms = {} 

@app.websocket("/ws/{room_id}")
async def websocket_endpoint(websocket: WebSocket, room_id: str):
    await websocket.accept()
    if room_id not in rooms:
        rooms[room_id] = []
    rooms[room_id].append(websocket)
    
    print(f"Peer joined room {room_id}")

    try:
        while True:
            # Receive JSON signaling data from a peer
            data = await websocket.receive_text()
            
            # Broadcast to everyone else in the room
            for client in rooms[room_id]:
                if client != websocket:
                    await client.send_text(data)
    except WebSocketDisconnect:
        rooms[room_id].remove(websocket)
        print(f"Peer left room {room_id}")

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)