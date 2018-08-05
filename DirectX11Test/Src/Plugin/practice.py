#ジェネレーター
#https://qiita.com/keitakurita/items/5a31b902db6adfa45a70
#https://qiita.com/Kodaira_/items/32b1ef860f59df80eedb
def GeneratorTestCountUP():
    i = 0
    while True:
        yield i
        i += 1

#ジェネレーターを使用したコルーチン
def Coroutine():
    #値が送信されてくる
    hello = yield "Hello"
    yield hello

import asyncio
import datetime
import random
#非同期asyncio コルーチン
#https://qiita.com/Kodaira_/items/32b1ef860f59df80eedb
@asyncio.coroutine
def DisplayDate0(num,loop):
    endTime = loop.time() + 50.0
    while True:
        print("Loop: {} Time: {}".format(num, datetime.datetime.now()))
        if (loop.time() + 1.0) >= endTime:
            break
        #ランダムな時間を待つ
        yield from asyncio.sleep(random.randint(0, 5))

#ネイティブコルーチン async/await
async def DisplayDate1(num, loop, ):
    endTime = loop.time() + 50.0
    while True:
        print("Loop: {} Time: {}".format(num, datetime.datetime.now()))
        if (loop.time() + 1.0) >= endTime:
            break
        #ランダムな時間を待つ
        await asyncio.sleep(random.randint(0, 5))

#ジェネレーターベースコルーチンとネイティブコルーチンの相互運用
import types 
@types.coroutine
def WaitForSecond(time):
    #ランダムな時間を待つ
    yield from asyncio.sleep(time)
async def DisplayDate2(num, loop, ):
    endTime = loop.time() + 50.0
    while True:
        print("Loop: {} Time: {}".format(num, datetime.datetime.now()))
        if (loop.time() + 1.0) >= endTime:
            break
        #ランダムな時間を待つ
        await WaitForSecond(2.0)


def Initialize():
    # #ジェネレーターテスト
    # gen = GeneratorTestCountUP()
    # for i in range(5):        
    #     #0~4
    #     print(next(gen))
    # for i in range(5):        
    #     #5~9
    #     print(next(gen))
    # c = Coroutine()
    # print(next(c))
    # #ジェネレーターに値を送信
    # print(c.send("World"))
    # #非同期asyncioコルーチン
    # loop0 = asyncio.get_event_loop()
    # asyncio.ensure_future(DisplayDate0("gen coroutine 0",loop0))
    # asyncio.ensure_future(DisplayDate0("gen coroutine 0",loop0))
    # #ここがメインとなる
    # loop0.run_forever()
    # #ネイティブコルーチン async/await
    # loop1 = asyncio.get_event_loop()
    # asyncio.ensure_future(DisplayDate1("nat coroutine 0", loop1))
    # asyncio.ensure_future(DisplayDate1("nat coroutine 0", loop1))
    # loop1.run_forever()
    #ジェネレーターベースコルーチンとネイティブコルーチンの相互運用
    # loop2 = asyncio.get_event_loop()
    # asyncio.ensure_future(DisplayDate2("gen&nat coroutine 0", loop2))
    # asyncio.ensure_future(DisplayDate2("gen&nat coroutine 1", loop2))
    # loop2.run_forever()
    pass
    
if __name__ in "__main__":
    Initialize()