#ifndef __LOG_H__
#define __LOG_H__

#define FUNC    (char*)__FUNCTION__

typedef enum tagLOG_Lv
{
    LOG_NONE,
    LOG_TRACE,    
    LOG_DBG,
    LOG_MSG,
    LOG_WARN,
    LOG_ERR,
    LOG_FATAL,
}LOG_Lv;

extern  int sys_log(char* module, LOG_Lv level, char* format, ...);

#define LOG_ON


#ifdef LOG_ON
#define log_debug(f, x)		sys_log(FUNC, LOG_DBG, f, x)
#define log_trace(f, x)		sys_log(FUNC, LOG_TRACE, f, x)
#define log_msg(f, x)		sys_log(FUNC, LOG_MSG, f, x)
#define log_warn(f, x)		sys_log(FUNC, LOG_WARN, f, x)
#define log_err(f, x)			sys_log(FUNC, LOG_ERR, f, x)
#define log_fatal(f, x)		sys_log(FUNC, LOG_FATAL, f, x)


#define log_d()		sys_log(FUNC, LOG_DBG, "")
#define log_t()		sys_log(FUNC, LOG_TRACE, "")
#define log_m()		sys_log(FUNC, LOG_MSG, "")
#define log_w()		sys_log(FUNC, LOG_WARN,"")
#define log_e()			sys_log(FUNC, LOG_ERR,"")
#define log_f()		sys_log(FUNC, LOG_FATAL, "")


#else
#define log_debug(f, ...)		
#define log_trace(f, ...)		
#define log_msg(f, ...)		
#define log_warn(f, ...)		
#define log_err(f, ...)			
#define log_fatal(f, ...)


#define log_d()		
#define log_t()		
#define log_m()	
#define log_w()	
#define log_e()		
#define log_f()	


#endif/*LOG_ON*/

#endif/*__LOG_H__*/
