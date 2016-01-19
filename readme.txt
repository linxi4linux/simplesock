为了进行gxp流程的测试，用socket技术实现的简单的客户端和服务端。
socketclient和socketserver均采用了多进程的方式。


#####socketclient: useage()
@支持多进程和单进程。多进程默认进行1笔交易
@支持stdin输入和文件输入;文件输入格式： <<filename


#####socketserver:useage()
@多进程式的服务端,收到内容，返回收到的内容


####server_select:useage()
@采用select方式的io复用策略

###sv_select:useage()
@采用select 方式的io复用策略,同server_select.
