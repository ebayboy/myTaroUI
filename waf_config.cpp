/**
 * @file: waf_config.cpp
 * @desc:
 *
 * Zhang hongyu,  2018/09/14
 *
 * Copyright (c) 2018, jd.com.
 * Copyright (c) 2018, jdcloud.com
 * All rights reserved.
 **/

#include <string>


#ifdef __cplusplus
extern "C" {
#endif
#include "hs.h"
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_conf_file.h>
#include "waf_config.h"
#include "waf_shm.h"
#include "cJSON.h"
#include "waf_log.h"
#include "waf_common.h"
#include "waf_cc.h"
#include "waf_antispider.h"
#include "waf_bot.h"
#include "waf_weblock.h"
#include "waf_dlp.h"
#include "ngx_http_waf_module.h"
#include "ngx_http_waf_evil_ip.h"
#include "ngx_http_waf_ip_region.h"
#include "ngx_http_waf_ip_region_rbtree.h"
#include "ngx_http_waf_rfc2616.h"

#ifdef __cplusplus
}
#endif

#include "ngx_http_waf_ctx.h"
#include "ngx_http_waf_urule.h"
#include "ngx_http_waf_match.h"

#define WAF_CONF_VERSION 1
#define WAF_CONF_BID_DATA_VERSION 1

#define CC_CUSTOM_POLICY_MAX       1000
#define CC_POLICY_MODE_FREQUENCY   1
#define CC_POLICY_MODE_RESP_TIME   2 

#define INTR_RULES_MAX             1024
#define USER_RULES_MAX             1024
#define SEC_RULES_MAX              1024

#define USER_RULES_HS_FLAGS         (HS_FLAG_CASELESS    | \
                                     HS_FLAG_SINGLEMATCH | \
                                     HS_FLAG_DOTALL)

#define SEC_RULES_HS_FLAGS          (HS_FLAG_CASELESS    | \
                                     HS_FLAG_SINGLEMATCH | \
                                     HS_FLAG_DOTALL)

/* HS_FLAG_SOM_LEFTMOST是为了DLP功能获取匹配成功后的起始位置 
 * 因这个FLAG会消耗更多性能，因此只在内置规则使用
 */
#define INTER_RULES_HS_FLAGS        (HS_FLAG_CASELESS    | \
                                     HS_FLAG_DOTALL      | \
                                     HS_FLAG_SOM_LEFTMOST)   

typedef struct waf_cf_metadata_s {    
    ngx_conf_t              *cf;
    waf_variable_comb_t     *var_comb;
} waf_cf_metadata_t;

waf_big_data_t  big_data_global;
waf_big_data_t  *big_data = &big_data_global;
ngx_pool_t      *big_data_pool;

static ngx_int_t 
waf_value_ngx_str_copy(ngx_pool_t *pool, cJSON *cmd, void *data);
static ngx_int_t 
waf_action_str_to_enum(ngx_pool_t *pool, cJSON *cmd, void *action);
static ngx_int_t 
waf_value_time_str_to_sec_int(ngx_pool_t *pool, cJSON *cmd, void *data);
static void *
waf_load_big_data_version(cJSON *root, ngx_pool_t *pool);
static void *
waf_load_version(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void *
waf_load_conf_global(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void * 
waf_load_conf_white_list(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void *
waf_load_conf_sa(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void * 
waf_load_conf_cc(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void *
waf_load_conf_spider(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void * 
waf_load_conf_bruteforce(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void * 
waf_load_conf_bot(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void * 
waf_load_conf_dlp(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void * 
waf_load_conf_weblock(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void *
waf_load_conf_waf_monitor(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void *
waf_load_conf_malicous_ip(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);

static void * waf_load_conf_ip_region(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);
static void * waf_load_conf_ip_region_rbtree(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);

static void * waf_load_conf_rfc2616(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);

typedef ngx_int_t (*value_handler_t)(ngx_pool_t *pool, cJSON *cmd, void *data);

static void * waf_load_conf_user_rules(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);

static void * waf_load_conf_sec_rules(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf);

/* ------------------------对外接口起始---------------------------------------- */
cJSON *
waf_get_node(cJSON *root, const char *first, ...)
{
    cJSON   *json_node = root;
    char    *menu;
    va_list ap_list;
    menu =  (char *)first;

    va_start(ap_list, first);

    do {

        json_node = cJSON_GetObjectItem(json_node, menu);
        menu = va_arg(ap_list, char *); 
        
        if (menu == NULL) {
            return json_node;
        }

    } while (menu != NULL );

    va_end(ap_list);

    return NULL;
}

ngx_int_t
waf_get_cmd(cJSON *root, value_handler_t handler, ngx_pool_t *pool, void *data, 
    const char *first, ...)
{
    cJSON *json_node = root;
    char *menu;
    va_list ap_list;
    menu = (char *)first;

    va_start(ap_list, first);

    do {

        json_node = cJSON_GetObjectItem(json_node, menu);

        if (json_node == NULL) {
            return NGX_ERROR;
        }

        menu = va_arg(ap_list, char *); 
        if (menu == NULL && json_node != NULL) {

            /* value值为空 */
            if (json_node->valuestring == NULL || 
                strlen(json_node->valuestring) == 0) {
                return NGX_ERROR;
            }

            if (handler(pool, json_node, data) != NGX_OK) {
                return NGX_ERROR;
            }

            return NGX_OK;
        }

    } while (menu != NULL );

    va_end(ap_list);

    return NGX_OK;
}

ngx_int_t waf_get_array_size(cJSON *root, const char *first, ...)
{
    cJSON *json_node = root;
    char *menu;
    va_list ap_list;
    menu = (char *)first;
    int size;

    va_start(ap_list, first);

    do {

        json_node = cJSON_GetObjectItem(json_node, menu);
        menu = va_arg(ap_list, char *); 

        if (menu == NULL) {
            size = cJSON_GetArraySize(json_node);
            return size;
        }

    } while (menu != NULL );

    va_end(ap_list);

    return 0;
}

static void
waf_conf_file_decrypt(char *data, ngx_int_t len)
{
    ngx_int_t n, m;
    char *mask = (char *)CONF_ENCRYPT_MASK;

    for (n = 0; n < len; n++) {
        m = n % (sizeof(CONF_ENCRYPT_MASK) - 1);
        data[n] ^= mask[m];
    }
}

/* 读取当前引擎对应版本的配置文件 */
static void * 
waf_load_curr_big_data_conf(cJSON *root, ngx_pool_t *pool)
{
    cJSON             *bigdata, *item, *addrs_node, *sub_item,
                      *addr_node;
    ngx_int_t         cnt, i, j, rc;
    big_data_policy_t *policy;
    ngx_str_t         addr_str;
    ngx_cidr_t        *cidr;

    bigdata = waf_get_node(root, "BigData", NULL);
    if (bigdata == NULL) {
        WAF_LOG(NOTICE, pool->log, "no BigData cmd in waf_db.dat..."
            "skip the big data protection");
        return NGX_OK;
    }

    cnt = waf_get_array_size(bigdata, "Policy", NULL);
    if (cnt > 0) {

        if (big_data->policies == NULL) {
            big_data->policies = 
                          ngx_array_create(pool, 10, sizeof(big_data_policy_t));

            if (big_data->policies == NULL) {
                return NGX_CONF_ERROR;
            }
        }

        item = waf_get_node(bigdata, "Policy", NULL);

        for (i = 0; i < cnt; i++) {

            policy = (big_data_policy_t *)ngx_array_push(big_data->policies);
            if (policy == NULL) {
                return NGX_CONF_ERROR;
            }

            ngx_memzero(policy, sizeof(big_data_policy_t));

            sub_item  = cJSON_GetArrayItem(item, i);

            rc = waf_get_cmd(sub_item, waf_value_ngx_str_copy, pool, 
                                   &policy->id, "id", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            WAF_LOG(NOTICE, pool->log, "bigdata policy id: [%V]", &policy->id);  

            rc = waf_get_cmd(sub_item, waf_value_ngx_str_copy, pool, 
                                   &policy->type, "type", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            WAF_LOG(NOTICE, pool->log, "bigdata policy type: [%V]", &policy->type);

            addrs_node = waf_get_node(sub_item, "addrs", NULL);

            if (addrs_node == NULL) {
                WAF_LOG(ERR, pool->log, "load big data conf failed");
                return NGX_CONF_ERROR;
            }

            ngx_int_t addr_cnt = waf_get_array_size(sub_item, "addrs", NULL);

            if (addr_cnt == 0) {
                return NGX_CONF_ERROR;
            }

            policy->addrs = ngx_array_create(pool, 3, sizeof(ngx_cidr_t));
            if (policy->addrs == NULL) {
                WAF_LOG(ERR, pool->log, "load big data conf failed");
                return NGX_CONF_ERROR;
            }
        
            for (j = 0; j < addr_cnt; j++) {

                addr_node  = cJSON_GetArrayItem(addrs_node, j);
                if (addr_node == NULL) {
                    return NGX_CONF_ERROR;
                }

                addr_str.data = (u_char *)cJSON_GetStringValue(addr_node);
                if (addr_str.data == NULL) {
                    continue;
                }

                addr_str.len = ngx_strlen(addr_str.data);

                if ((cidr = (ngx_cidr_t *)ngx_array_push(policy->addrs)) == NULL) {
                    WAF_LOG(ERR, pool->log, "ngx_array_push cdir error.");
                    return NGX_CONF_ERROR;
                }

                ngx_memzero(cidr, sizeof(ngx_cidr_t));
                
                if (ngx_ptocidr(&addr_str, cidr) == NGX_ERROR) {
                    return NGX_CONF_ERROR;
                }

                WAF_LOG(NOTICE, pool->log, "addr: [%V]", &addr_str);
            }

            rc = waf_get_cmd(sub_item, waf_action_str_to_enum, pool, 
                             &policy->action, "action", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            rc = waf_get_cmd(sub_item, waf_value_time_str_to_sec_int, 
                             pool, &policy->expired, "expired", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            WAF_LOG(NOTICE, pool->log, "bigdata policy action: [%V]",  &policy->type);      
            WAF_LOG(NOTICE, pool->log, "bigdata policy expired: [%d]", policy->expired);
        }
    }

    return NGX_OK;
}

static void * 
waf_load_curr_conf(ngx_conf_t *cf, ngx_cycle_t *cycle, waf_main_conf_t *waf_conf,
                   cJSON *root)
{
    void *rc;

    /* 读取全局配置 */
    rc = waf_load_conf_global(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    if (!waf_conf->WafEngine) {
        /* 全局开关off, 不读取后续配置 */
        WAF_LOG(NOTICE, cf->log, "WAF off, skip waf function", __func__);
        return NGX_OK;
    }

    /* 读取语义解析引擎配置 */
    rc = waf_load_conf_sa(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    /* 读取白名单配置 */
    rc = waf_load_conf_white_list(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    /* 读取防CC配置 */
    rc = waf_load_conf_cc(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    /* 读取防爬虫配置 */
    rc = waf_load_conf_spider(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    /* 读取防暴力破解配置 */
    rc = waf_load_conf_bruteforce(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    /* 读取防BOT配置 */
    rc = waf_load_conf_bot(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    /* 读取数据防泄漏配置 */
    rc = waf_load_conf_dlp(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    /* 读取网页防篡改配置 */
    rc = waf_load_conf_weblock(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    /* 读取WAF用户自定义规则 */
    rc = waf_load_conf_user_rules(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    /* 读取WAF安全规则 */
    rc = waf_load_conf_sec_rules(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    /* 读取WAF引擎监控配置 */
    rc = waf_load_conf_waf_monitor(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    /* 读取恶意IP自动封禁配置 */
    rc = waf_load_conf_malicous_ip(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

#if 0
    /* 读取IP地域封禁配置 */
    rc = waf_load_conf_ip_region(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }
#else
    /* 读取IP地域封禁配置 */
    rc = waf_load_conf_ip_region_rbtree(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }
#endif

    /* read rfc2616 check configuration */
    rc = waf_load_conf_rfc2616(cf, root, waf_conf);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        return NGX_CONF_ERROR;
    }

    return NGX_OK;
}

/* 读取大数据引擎下发的风险配置文件.
 * Note: 因不能用nginx reload的流程，因此不能使用nginx cycle, ngx_conf_t等结构
 */
void * 
waf_load_big_data_conf(ngx_log_t *log)
{
    FILE       *conf_file = NULL;
    ngx_int_t  len;
    void       *rc;
    char       *data;
    cJSON      *root = NULL;

    if (big_data_pool == NULL) {

        big_data_pool = ngx_create_pool(20 * 1024 * 1024, log);
        if (big_data_pool == NULL) {
            return NGX_CONF_ERROR;
        }

    } else {
        /* 周期性reload前清空已有配置 */
        ngx_reset_pool(big_data_pool);
        big_data->policies = NULL;

        ngx_memzero(big_data, sizeof(waf_big_data_t));
    }

    big_data_pool->log = log;

    conf_file = fopen(WAF_CONF_BIG_DATA_PATH, "rb+");

    if (conf_file == NULL) {
        // WAF_LOG(NOTICE, cf->log, "Can not find %s,"
        //      "skip WAF big data engine...", WAF_CONF_BIG_DATA_PATH);
        return NGX_OK;
    }

    /* json格式配置文件 */
    fseek(conf_file, 0, SEEK_END);
    len = ftell(conf_file);
    fseek(conf_file, 0, SEEK_SET);

    data = (char*) ngx_pcalloc(big_data_pool, len + 1);
    if (data == NULL) {
        goto failed;
    }

    if (fread(data, len, 1, conf_file) != 1) {
        // WAF_LOG(ERR, cf->log, "read waf_bd.dat to data failed");

        goto failed;
    }

    if ((root = cJSON_Parse(data)) == NULL) {
        /* 将混淆过的配置文件还原为json格式 */
        waf_conf_file_decrypt(data, len);
    }

    root = cJSON_Parse(data);
    if (root == NULL) {
        // WAF_LOG(ERR, cf->log, "Invalid JSON format in waf_db.dat [%s]",
        //     cJSON_GetErrorPtr());
        goto failed;
    }

    /* 读取当前配置文件的版本 */
    rc = waf_load_big_data_version(root, big_data_pool);
    if (rc != NGX_OK) {
        // WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        goto failed;
    }

    if (WAF_CONF_BID_DATA_VERSION == big_data->version) {

        /* 新引擎，新配置 */
        rc = waf_load_curr_big_data_conf(root, big_data_pool);

        if (rc != NGX_OK) {
            // WAF_LOG(ERR, cf->log, "waf_load_conf failed");
            goto failed;
        }

    } else {

        // WAF_LOG(ERR, cf->log,
        //     "WAF engine can't compatible with the big data config file which verison"
        //     "is %d. Current WAF engine version is %d. SKip WAF function...",
        //      waf_conf->bd_version, WAF_CONF_BID_DATA_VERSION);
    }

    fclose(conf_file);
    cJSON_Delete(root);

    return NGX_OK;

failed:
    if (conf_file != NULL) {
        fclose(conf_file);
    }

    if (root != NULL) {
        cJSON_Delete(root);
    }

    ngx_reset_pool(big_data_pool);
    big_data->policies = NULL;

    return NGX_CONF_ERROR;
}

extern "C" void * 
waf_load_conf(ngx_conf_t *cf, ngx_cycle_t *cycle, waf_main_conf_t *waf_conf)
{
    FILE      *conf_file;
    ngx_int_t  len;
    void      *rc;
    char      *data;
    cJSON     *root;
    char      path[1024];

    snprintf(path, sizeof(path) - 1, "%.*s/waf.dat", (int)cycle->conf_prefix.len,
            cycle->conf_prefix.data);

    conf_file = fopen(path, "rb+");

    if (conf_file == NULL) {
        WAF_LOG(ERR, cf->log, "Can not find waf.dat in %s, skip WAF...", path);
        waf_conf->WafEngine = 0;
        return NGX_OK;
    }

    /* json格式配置文件 */
    fseek(conf_file, 0, SEEK_END);
    len = ftell(conf_file);
    fseek(conf_file, 0, SEEK_SET);

    data = (char*) ngx_pcalloc(cf->pool, len + 1);
    if (data == NULL) {
        fclose(conf_file);
        return NGX_CONF_ERROR;
    }

    if (fread(data, len, 1, conf_file) != 1) {
        WAF_LOG(ERR, cf->log, "read waf.dat to data failed");
        fclose(conf_file);
        return NGX_CONF_ERROR;
    }

    if ((root = cJSON_Parse(data)) == NULL) {
        /* 将混淆过的配置文件还原为json格式 */
        waf_conf_file_decrypt(data, len);
    }

    root = cJSON_Parse(data);
    if (root == NULL) {
        WAF_LOG(ERR, cf->log, "Invalid JSON format in waf.dat [%s]",
            cJSON_GetErrorPtr());
        fclose(conf_file);
        return NGX_CONF_ERROR;
    }

    /* 读取当前配置文件的版本 */
    rc = waf_load_version(cf, root, waf_conf);
    if (rc != NGX_OK) {
#if 0  /* 因2.2版没有version字段，暂时注掉，2.3版上线后打开 */
        WAF_LOG(ERR, cf->log, "func [%s] failed", __func__);
        fclose(conf_file);
        cJSON_Delete(root);
        return NGX_CONF_ERROR;
#endif
    }

    /* 注意: 修改配置解析函数时避免影响解析老的配置，
     * 如有必要，将解析函数单独复制一份进行修改
     */
    if (WAF_CONF_VERSION == waf_conf->version) {

        /* 新引擎，新配置 */
        rc = waf_load_curr_conf(cf, cycle, waf_conf, root);

        if (rc != NGX_OK) {
            WAF_LOG(ERR, cf->log, "waf_load_conf failed");

            fclose(conf_file);
            cJSON_Delete(root);

            return NGX_CONF_ERROR;
        }

    } else {

        WAF_LOG(ERR, cf->log,
            "WAF engine can't compatible with the config file which verison"
            "is %d. Current WAF engine version is %d. SKip WAF function...",
             waf_conf->version, WAF_CONF_VERSION);
        waf_conf->WafEngine = 0;

        /* 跳过WAF功能 */
    }

    fclose(conf_file);
    cJSON_Delete(root);

    return NGX_OK;
}

/* ------------------------对外接口结束---------------------------------------- */

/* ------------------------对内接口起始---------------------------------------- */
#if 0
static
ngx_int_t waf_load_rule(ngx_conf_t *cf, waf_main_conf_t *waf_conf,
                             wafMatch_t *matcher, waf_rule_t *rule,
                        uint32_t hs_flags) 
{
    std::string   mz_std;
    char          buff[128];
    ngx_str_t     *one_mz;
    match_t       *m = NULL;
    ngx_int_t     n;
    uint32_t      rule_id;
    char          *rx_buff;

    ngx_memzero(buff, sizeof(buff));
    sprintf(buff, "%.*s", (int)rule->id.len, rule->id.data);

    rule_id = atoi(buff);
    if (rule_id == 0) {
        WAF_LOG(ERR, cf->log, "Invalid internal rule id");
        return NGX_ERROR;
    }

    for (n = 0; n < (int)rule->mz_array->nelts; n++) {

        one_mz = &((ngx_str_t *)rule->mz_array->elts)[n];

        mz_std.assign((const char *)one_mz->data, one_mz->len);

        if ((m = matcher->find(cf, mz_std)) == NULL) {
            WAF_LOG(ERR, cf->log, "inernal rule find matchZone error.");
            return NGX_ERROR;
        }

        if (m->h == NULL) {
            if ((m->h = waf_filter_new()) == NULL) {
                WAF_LOG(ERR, cf->log, "waf_filter_new() error.");
                return NGX_ERROR;
            }
        }

        rx_buff = (char *)ngx_pcalloc(cf->pool, rule->rx.len + 1);
        if (rx_buff == NULL) {
            WAF_LOG(ERR, cf->log, "alloc rx_buff failed");
            return NGX_ERROR;
        }

        strncpy(rx_buff, (const char *)rule->rx.data, rule->rx.len);

        if (waf_filter_addrule(m->h, rule_id, rx_buff, hs_flags) != 0) {
            return NGX_ERROR;
        }
    }

    return NGX_OK;
}
#endif

static
ngx_int_t waf_switch_str_to_enum(ngx_pool_t *pool, cJSON *cmd, void *data)
{
    if (strcasecmp(cmd->valuestring, "on") == 0) {
        *(uint8_t *)data = TRUE;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "enable") == 0) {
        *(uint8_t *)data = TRUE;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "enabled") == 0) {
        *(uint8_t *)data = TRUE;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "off") == 0) {
        *(uint8_t *)data = FALSE;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "disable") == 0) {
        *(uint8_t *)data = FALSE;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "disabled") == 0) {
        *(uint8_t *)data = FALSE;
        return NGX_OK;
    }

    return NGX_ERROR;
}

static
ngx_int_t waf_action_str_to_enum(ngx_pool_t *pool, cJSON *cmd, void *action)
{
    if (strcasecmp(cmd->valuestring, "none") == 0) {
        *(waf_action_e *)action = WAF_ACT_NONE;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "log") == 0) {
        *(waf_action_e *)action = WAF_ACT_LOG;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "pass") == 0) {
        *(waf_action_e *)action = WAF_ACT_PASS;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "block") == 0) {
        *(waf_action_e *)action = WAF_ACT_BLOCK;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "challenge_cookie") == 0) {
        *(waf_action_e *)action = WAF_ACT_CHALLENGE_COOKIE;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "challenge_js") == 0) {
        *(waf_action_e *)action = WAF_ACT_CHALLENGE_JS;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "challenge_vfcode") == 0) {
        *(waf_action_e *)action = WAF_ACT_CHALLENGE_VFCODE;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "mask") == 0) {
        *(waf_action_e *)action = WAF_ACT_MASK;
        return NGX_OK;
    }

    return NGX_ERROR;
}

static
ngx_int_t waf_value_ngx_str_copy(ngx_pool_t *pool, cJSON *cmd, void *data)
{
    u_char *buff;

    buff = (u_char *)ngx_pcalloc(pool, strlen(cmd->valuestring) + 1);
    if (buff == NULL) {
        return NGX_ERROR;
    }

    strcpy((char *)buff, cmd->valuestring);

   ((ngx_str_t *)data)->data = buff;
   ((ngx_str_t *)data)->len  = strlen(cmd->valuestring);

    return NGX_OK;
}

static 
ngx_int_t waf_parse_dlp_mode(ngx_pool_t *pool, cJSON *cmd, void *data)
{
    if (strcmp(cmd->valuestring, "content") == 0) {
        *(uint8_t *)data = DLP_MODE_CONTENT;
    } else if (strcmp(cmd->valuestring, "errorcode") == 0) {
        *(uint8_t *)data = DLP_MODE_ERRCODE;
    } else {
        return NGX_ERROR;
    }

    return NGX_OK;
}

__attribute__((unused)) static
ngx_int_t waf_value_std_str_copy(ngx_pool_t *pool, cJSON *cmd, void *data)
{
    u_char *buff;

    buff = (u_char *)ngx_pcalloc(pool, strlen(cmd->valuestring) + 1);
    if (buff == NULL) {
        return NGX_ERROR;
    }

    strcpy((char *)buff, cmd->valuestring);

   ((std::string *)data)->assign((const char *)buff, strlen((const char*)buff));

    return NGX_OK;
}

#if 0
static
ngx_int_t waf_value_logic_to_enum(ngx_pool_t *pool, cJSON *cmd, void *data)
{
    if (strcasecmp(cmd->valuestring, "y") == 0) {
        *(uint32_t *)data = WAF_LOGIC_CONTAIN;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "n") == 0) {
        *(uint32_t *)data = WAF_LOGIC_NOTCONTAIN;
        return NGX_OK;
    }

    return NGX_ERROR;
}
#endif

static
ngx_int_t waf_parse_freq_base_threshold(ngx_pool_t *pool, cJSON *cmd, void *data)
{
    char reqs[128], interval[128];
    waf_threshold_t *threshold = (waf_threshold_t *)data;

    ngx_memzero(reqs, sizeof(reqs));
    ngx_memzero(interval, sizeof(interval));

    sscanf((char *) cmd->valuestring, "%[0-9k?]/%[0-9(s|m)?]", reqs, interval);

    threshold->freq_base_times = atoi(reqs);
    if (reqs[strlen(reqs) - 1] == 'k' ||
        reqs[strlen(reqs) - 1] == 'K') {
        threshold->freq_base_times *= 1000;
    }

    threshold->freq_base_interval = atoi(interval);
    if (threshold->freq_base_interval == 0) {
        threshold->freq_base_interval = 1;
    }

    if (interval[strlen(interval) - 1] == 'm' ||
        interval[strlen(interval) - 1] == 'M') {
        threshold->freq_base_interval *= 60;
    }

    if (interval[strlen(interval) - 1] == 'h' ||
        interval[strlen(interval) - 1] == 'H') {
        threshold->freq_base_interval *= 3600;
    }

    return NGX_OK;
}

static
ngx_int_t waf_parse_time_base_threshold(ngx_pool_t *pool, cJSON *cmd, void *data)
{
    char ms[128];
    waf_threshold_t *threshold = (waf_threshold_t *)data;

    ngx_memzero(ms, sizeof(ms));

    sscanf((char *) cmd->valuestring, "%[0-9(s|S|m|M|h|H|ms|MS)?]", ms);

    threshold->time_base_ms = atoi(ms);
    /* ms */
    if ((ms[strlen(ms) - 2] == 'm' ||
        ms[strlen(ms) - 2] == 'M') &&
        (ms[strlen(ms) - 1] == 's'||
        ms[strlen(ms) - 1] == 'S')) {
        return NGX_OK;
    }

    /* sec to ms */
    if (ms[strlen(ms) - 1] == 's'||
        ms[strlen(ms) - 1] == 'S') {
        threshold->time_base_ms *= 1000;
    }

    /* minutes to ms */
    if (ms[strlen(ms) - 1] == 'm'||
        ms[strlen(ms) - 1] == 'M') {
        threshold->time_base_ms *= (60 * 1000);
    }

    /* hours to ms */
    if (ms[strlen(ms) - 1] == 'h'||
        ms[strlen(ms) - 1] == 'H') {
        threshold->time_base_ms *= (60 * 60 * 1000);
    }

    return NGX_OK;
}

static
ngx_int_t waf_value_time_str_to_sec_int(ngx_pool_t *pool,
                cJSON *cmd, void *data)
{
    char *interval;

    interval = (char *)ngx_pcalloc(pool, strlen(cmd->valuestring) + 1);
    if (interval == NULL) {
        return NGX_ERROR;
    }

    sscanf((char *)cmd->valuestring, "%[0-9(s|m|ms)?]", interval);

    *(ngx_int_t *)data = atol(interval);
    if (*(ngx_int_t *)data < 0) {
        return NGX_ERROR;
    }

    if (interval[strlen(interval) - 1] == 'm' ||
        interval[strlen(interval) - 1] == 'M') {
        *(ngx_int_t *)data *= 60;
    }

    return NGX_OK;
}


static
ngx_int_t waf_value_file_size_str_to_byte_int(ngx_pool_t *pool,
                cJSON *cmd, void *data)
{
    char *interval;

    interval = (char *)ngx_pcalloc(pool, strlen(cmd->valuestring) + 1);
    if (interval == NULL) {
        return NGX_ERROR;
    }

    sscanf((char *)cmd->valuestring, "%[0-9(b|B|m|M|g|G)?]", interval);

    *(ngx_int_t *)data = atoi(interval);
    if (*(ngx_int_t *)data < 0) {
        return NGX_ERROR;
    }

    if (interval[strlen(interval) - 1] == 'm' ||
        interval[strlen(interval) - 1] == 'M') {
        *(ngx_int_t *)data *= 1024 * 1024;
    }

    if (interval[strlen(interval) - 1] == 'g' ||
        interval[strlen(interval) - 1] == 'G') {
        *(ngx_int_t *)data *= 1024 * 1024 * 1024;
    }

    return NGX_OK;
}

static
ngx_int_t waf_value_str_to_int(ngx_pool_t *pool, cJSON *cmd, void *data)
{
    *(ngx_int_t *)data = atoi(cmd->valuestring);

    if (*(ngx_int_t *)data < 0) {
        return NGX_ERROR;
    }

    return NGX_OK;
}

static void *
waf_load_version(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf)
{
    ngx_int_t rc;

    rc = waf_get_cmd(root,  waf_value_str_to_int, cf->pool, 
                     &waf_conf->version, "Version", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    WAF_LOG(NOTICE, cf->log, "Version [%d]", waf_conf->version);
    return NGX_OK;
}

static void *
waf_load_big_data_version(cJSON *root, ngx_pool_t *pool)
{
    ngx_int_t  rc;

    rc = waf_get_cmd(root,  waf_value_str_to_int, pool, 
                     &big_data->version, "Version", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    WAF_LOG(NOTICE, pool->log, "BigData Version [%d]", big_data->version);
    return NGX_OK;
}

static void *
waf_load_conf_global(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf)
{
    ngx_int_t rc;

    rc = waf_get_cmd(root,  waf_switch_str_to_enum, cf->pool, &waf_conf->WafEngine, "WafEngine", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(root, waf_value_ngx_str_copy, cf->pool, &waf_conf->WafId, "WafId", NULL);
    if (rc != NGX_OK) {
        /* Waf enable off情况下, WafId可能下发， 也可能不下发, 所以返回NGX_OK */
        return NGX_OK;
    }

    if (!waf_conf->WafEngine) {
        /* 全局开关off, 不读取后续配置 */
        return NGX_OK;
    }

    rc = waf_get_cmd(root, waf_action_str_to_enum, cf->pool,
                     &waf_conf->WafAction, "WafAction", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(root, waf_value_ngx_str_copy, cf->pool,
                     &waf_conf->challenge_key, "ChallengeKey", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(root, waf_switch_str_to_enum, cf->pool,
                     &waf_conf->is_by_cdn, "IsByJDCDN", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(root, waf_switch_str_to_enum, cf->pool,
                     &waf_conf->is_by_antiddos, "IsByJDAntiDDOS", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(root, waf_value_ngx_str_copy, cf->pool,
                     &waf_conf->custom_err_page, "CustomErrPage", NULL);
    if (rc != NGX_OK) {
        /* CustomErrPage的value可以为空 */
    }

    WAF_LOG(NOTICE, cf->log, "WafEngine    : [%d]", waf_conf->WafEngine);
    WAF_LOG(NOTICE, cf->log, "WafAction    : [%d]", waf_conf->WafAction);
    WAF_LOG(NOTICE, cf->log, "WafId        : [%V]", &waf_conf->WafId);
    WAF_LOG(NOTICE, cf->log, "ChallengeKey : [%V]", &waf_conf->challenge_key);
    WAF_LOG(NOTICE, cf->log, "IsByJDCDN      : [%d]", waf_conf->is_by_cdn);
    WAF_LOG(NOTICE, cf->log, "IsByJDAntiDDOS : [%d]", waf_conf->is_by_antiddos);
    WAF_LOG(NOTICE, cf->log, "CustomErrPage  : [%V]", &waf_conf->custom_err_page);

    return NGX_OK;
}

static void *
waf_load_conf_sa(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf)
{
    ngx_int_t rc, fp_cnt, i, j;
    cJSON     *sa_root, *fp_node, *sub_item;
    fingerprint_t *fp;

    sa_root = waf_get_node(root, "SA", NULL);
    if (sa_root == NULL) {
        return NGX_OK;
    }

    rc = waf_get_cmd(sa_root, waf_action_str_to_enum, cf->pool,
                     &waf_conf->SAAction, "SAAction", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    fp_cnt = waf_get_array_size(sa_root, "Fingerprint", NULL);

    if (fp_cnt > 0) {

        fp_node = waf_get_node(sa_root, "Fingerprint", NULL);

        if (fp_node == NULL) {
            return NGX_CONF_ERROR;
        }

        for (i = 0; i < fp_cnt; i++) {
            sub_item  = cJSON_GetArrayItem(fp_node, i);
            if (sub_item == NULL) {
                return NGX_CONF_ERROR;
            }

            fp = (fingerprint_t *)ngx_array_push(waf_conf->fingerprints);

            if (fp == NULL) {
                return NGX_CONF_ERROR;
            }

            ngx_memzero(fp, sizeof(fingerprint_t));

            rc = waf_get_cmd(sub_item, waf_value_ngx_str_copy, cf->pool,
                             &fp->value, 
                             "value", NULL);
            if (rc != NGX_OK || fp->value.len > 6) {
                return NGX_CONF_ERROR;
            }

            rc = waf_get_cmd(sub_item, waf_action_str_to_enum, cf->pool,
                             &fp->action, 
                             "action", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }
        }
    }

    WAF_LOG(NOTICE, cf->log, "SAEngine     : [%d]", waf_conf->SAEngine);
    WAF_LOG(NOTICE, cf->log, "SAAction     : [%d]", waf_conf->SAAction);

    for (j = 0; j < (int)waf_conf->fingerprints->nelts; j++) {
        fp = &((fingerprint_t *)waf_conf->fingerprints->elts)[j];
        WAF_LOG(NOTICE, cf->log, "Fingerprint: value [%V], action [%d]", 
               &fp->value, fp->action);      
    }

    return NGX_OK;
}

static
ngx_int_t waf_cc_policy_mode_to_enum(ngx_pool_t *pool, cJSON *cmd, void *mode)
{
    if (strcasecmp(cmd->valuestring, "resptime") == 0) {
        *(uint8_t *)mode = CC_POLICY_MODE_RESP_TIME;
        return NGX_OK;
    }

    if (strcasecmp(cmd->valuestring, "frequency") == 0) {
        *(uint8_t *)mode = CC_POLICY_MODE_FREQUENCY;
        return NGX_OK;
    }

    return NGX_ERROR;
}

/* 解析防护模块中哈希变量，形如: "$http_user_agent$remote_addr" */
static 
ngx_int_t waf_ngx_str_variable_split(ngx_pool_t *pool, 
                    cJSON *cmd, void *data)
{
    char                  *buffer, *begin;
    char                  *start;
    ngx_int_t             comb_len;
    waf_variable_comb_t   *var_comb;
    ngx_conf_t            *cf;

    cf        = ((waf_cf_metadata_t *)data)->cf;
    var_comb  = ((waf_cf_metadata_t *)data)->var_comb;


    if (strlen(cmd->valuestring) == 0) {
        /* 在cc自定义策略为基于响应时间的模式时，没有variable */
        return NGX_OK;
    }

    buffer = (char *)ngx_pcalloc(pool, strlen(cmd->valuestring) + 1);
    if (buffer == NULL) {
        return NGX_ERROR;
    }

    ngx_memcpy(buffer, cmd->valuestring, strlen(cmd->valuestring));
 

    begin = strtok(buffer, "|");
    if (begin == NULL) {
        /* 只有一个组合, 无"|"分隔符 */
        begin = buffer;
    }

    while (begin != NULL) {
        start = strchr(begin, '$');
        if (start == NULL) {
            return NGX_ERROR;
        }
        comb_len = strlen(start);

        var_comb->var.data = (u_char *)ngx_pcalloc(pool, comb_len);
        if (var_comb->var.data == NULL) {
            return NGX_ERROR;
        }

        ngx_memcpy(var_comb->var.data, start, comb_len);
        var_comb->var.len = comb_len;
        var_comb->sub_vars = ngx_array_create(pool, WAF_COMM_SUBVAR_MAX,
                sizeof(waf_variable_t));
        if (var_comb->sub_vars == NULL) {
            return NGX_ERROR;
        }

        if (waf_comm_parse_sub_var(cf, var_comb) != NGX_OK) {
            return NGX_ERROR;
        }

        begin = strtok(NULL, "|");
    }

    return NGX_OK;
}

#if 0
/* 解析安全规则中形如: "$http_user_agent, $remote_addr" */
static 
ngx_int_t waf_ngx_str_variable_split_dot(ngx_pool_t *pool, 
                    cJSON *cmd, void *data)
{
    char *buff, *start;
    ngx_str_t *var;
    void **array = (void **)data;
   
    buff = (char *)ngx_pcalloc(pool, strlen(cmd->valuestring) + 1);
    if (buff == NULL) {
        return NGX_ERROR;
    }

    strncpy(buff, cmd->valuestring, strlen(cmd->valuestring));

    *array = ngx_array_create(pool, 64, sizeof(ngx_str_t));
    if (*array == NULL) {
        return NGX_ERROR;
    }

    buff = strtok(buff, ",");

    do {
        start = strchr(buff, '$');
        if (start == NULL) {
            return NGX_ERROR;
        }

        var = (ngx_str_t *)ngx_array_push(*(ngx_array_t **)array);
        if (var == NULL) {
            return NGX_ERROR;
        }

        var->data = (u_char *)start;
        var->len  = strlen(start);

        buff = strtok(NULL, ",");
    } while (buff != NULL);

    return NGX_OK;
}
#endif

/* 解析规则ID组合 如: "20001|20002|20003*/
static 
ngx_int_t waf_rule_id_split(ngx_pool_t *pool, 
                    cJSON *cmd, void *data)
{
    char        *buff;
    ngx_str_t   *var;
    void        **array = (void **)data;

    if (strlen(cmd->valuestring) == 0) {
        return NGX_ERROR;
    }
   
    buff = (char *)ngx_pcalloc(pool, strlen(cmd->valuestring) + 1);
    if (buff == NULL) {
        return NGX_ERROR;
    }

    strncpy(buff, cmd->valuestring, strlen(cmd->valuestring));

    *array = ngx_array_create(pool, 64, sizeof(ngx_str_t));
    if (*array == NULL) {
        return NGX_ERROR;
    }

    buff = strtok(buff, "|");

    do {
        var = (ngx_str_t *)ngx_array_push(*(ngx_array_t **)array);
        if (var == NULL) {
            return NGX_ERROR;
        }

        var->data = (u_char *)buff;
        var->len  = strlen(buff);

        buff = strtok(NULL, "|");
    } while (buff != NULL);

    return NGX_OK;
}

static
ngx_int_t waf_weblock_disk_init(ngx_conf_t *cf, waf_main_conf_t *waf_conf)
{
    ngx_int_t             n, m, found = 0, len;
    DIR                   *dir;
    char                  *path;
    struct dirent         *ptr;

    web_lock_entry_t      *weblock, *old_weblock;

    waf_main_conf_t *old_conf = waf_conf->old_waf_main_conf;

    /* 查看新weblock比old weblock多的项 */
    for (n = 0; n < (int)waf_conf->weblock_array->nelts; n++) {
        weblock = &((web_lock_entry_t *)waf_conf->weblock_array->elts)[n];
        found = FALSE;

        if (old_conf != NULL && old_conf->weblock_array != NULL) {
            for (m = 0; m < (int)old_conf->weblock_array->nelts; m++) {

                old_weblock = &((web_lock_entry_t *)old_conf->weblock_array->elts)[m];
                if (weblock->url.len == old_weblock->url.len &&
                    ngx_strncmp((char *)weblock->url.data, (char *)old_weblock->url.data,
                            weblock->url.len) == 0) {
                    found = TRUE;
                    break;
                }
            }
        }

        if (found) {
            if (weblock->last_update != old_weblock->last_update) {
                waf_weblock_del_resource(cf->pool, weblock, FALSE);
            }
        } else {
            waf_weblock_del_resource(cf->pool, weblock, FALSE);
        }
    }

    /* 查看old weblock比weblock多的项 */
    if (old_conf != NULL && old_conf->weblock_array != NULL) {
        for (n = 0; n < (int)old_conf->weblock_array->nelts; n++) {
            old_weblock = &((web_lock_entry_t *) old_conf->weblock_array->elts)[n];
            found = FALSE;

            if (waf_conf->weblock_array->nelts > 0) {
                for (m = 0; m < (int)waf_conf->weblock_array->nelts; m++) {
                    weblock = &((web_lock_entry_t *)waf_conf->weblock_array->elts)[m];

                    if (weblock->url.len == old_weblock->url.len &&
                        ngx_strncmp((char *)weblock->url.data, (char *)old_weblock->url.data,
                                weblock->url.len) == 0) {
                        found = TRUE;
                        break;
                    }
                }
            }

            if (!found) {
                waf_weblock_del_resource(cf->pool, old_weblock, FALSE);
            }
        }
    }

    /* 查看现有磁盘缓存的文件既不在新的weblock也不在old weblock上 */
    dir = opendir(WEBLOCK_PATH);
    if (dir == NULL) {
        return NGX_OK;
    }

    while ((ptr = readdir(dir)) != NULL) {
         if (strcmp(ptr->d_name, ".") == 0 ||
             strcmp(ptr->d_name, "..") == 0) {
             continue;
         }

         found = FALSE;

         len = strlen(WEBLOCK_PATH) + strlen(ptr->d_name) + 2;

         path = (char *)ngx_pcalloc(cf->pool, len);
         if (path == NULL) {
             closedir(dir);
             return NGX_ERROR;
         }

         snprintf(path, len, "%s/%s", WEBLOCK_PATH, ptr->d_name);

         for (n = 0; n < (int)waf_conf->weblock_array->nelts; n++) {
             weblock = &((web_lock_entry_t *)waf_conf->weblock_array->elts)[n];
             if (strcmp((char *)weblock->resource_path, path) == 0) {
                 found = TRUE;
                 break;
             }
         }

         if (old_conf != NULL && old_conf->weblock_array != NULL) {
             for (n = 0; n < (int)old_conf->weblock_array->nelts; n++) {
                 weblock = &((web_lock_entry_t *) old_conf->weblock_array->elts)[n];
                 if (strcmp((char *)weblock->resource_path, path) == 0) {
                     found = TRUE;
                     break;
                 }
             }
         }

         if (!found) {
             remove(path);
         }
    }

    closedir(dir);

    return NGX_OK;
}

static
void * waf_load_conf_white_list(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf)
{
    ngx_int_t rc, cnt, i, j;
    cJSON     *wl_root, *entry_node, *sub_item;
    whitelist_t *wl;
    cJSON *addrs_node, *addr_node;
    ngx_cidr_t *cidr;
    ngx_str_t pstr;

    wl_root = waf_get_node(root, "WhiteList", NULL);
    if (wl_root == NULL) {
        return NGX_OK;
    }

    cnt = waf_get_array_size(wl_root, "WhiteListEntry", NULL);

    if (cnt > 0) {

        entry_node = waf_get_node(wl_root, "WhiteListEntry", NULL);

        if (entry_node == NULL) {
            return NGX_CONF_ERROR;
        }

        for (i = 0; i < cnt; i++) {

            sub_item  = cJSON_GetArrayItem(entry_node, i);
            if (sub_item == NULL) {
                return NGX_CONF_ERROR;
            }

            wl = (whitelist_t *)ngx_array_push(waf_conf->whitelist);

            if (wl == NULL) {
                return NGX_CONF_ERROR;
            }

            ngx_memzero(wl, sizeof(whitelist_t));

            rc = waf_get_cmd(sub_item, waf_value_ngx_str_copy, cf->pool,
                             &wl->id, 
                             "id", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            rc = waf_get_cmd(sub_item, waf_value_str_to_int, cf->pool,
                             &wl->match_id, 
                             "matchid", NULL);
            if (rc != NGX_OK) {
                /* pass through */
            }

            rc = waf_get_cmd(sub_item, waf_value_time_str_to_sec_int, 
                             cf->pool, &wl->expired, "date", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            ngx_int_t addr_cnt = waf_get_array_size(sub_item, "addrs", NULL);

            if (addr_cnt == 0 && wl->match_id == 0) {
                /* 无match_id和ip列表 */
                return NGX_CONF_ERROR;
            }

            addrs_node = waf_get_node(sub_item, "addrs", NULL);

            if (addrs_node == NULL) {
                continue;
            }

            wl->addrs = ngx_array_create(cf->pool, 3, sizeof(ngx_cidr_t));
            if (wl->addrs == NULL) {
                WAF_LOG(ERR, cf->log, "load white list failed");
                return NGX_CONF_ERROR;
            }
        
            for (j = 0; j < addr_cnt; j++) {

                addr_node  = cJSON_GetArrayItem(addrs_node, j);
                if (addr_node == NULL) {
                    return NGX_CONF_ERROR;
                }

                pstr.data = (u_char *)cJSON_GetStringValue(addr_node);
                if (pstr.data == NULL) {
                    continue;
                }

                pstr.len = ngx_strlen(pstr.data);

                if ((cidr = (ngx_cidr_t *)ngx_array_push(wl->addrs)) == NULL) {
                    WAF_LOG(ERR, cf->log, "ngx_array_push cdir error.");
                    return NGX_CONF_ERROR;
                }

                ngx_memzero(cidr, sizeof(ngx_cidr_t));
                
                if (ngx_ptocidr(&pstr, cidr) == NGX_ERROR) {
                    return NGX_CONF_ERROR;
                }

                WAF_LOG(NOTICE, cf->log, "addr: [%V]", &pstr);
            }
        }

        for (j = 0; j < (int)waf_conf->whitelist->nelts; j++) {

            wl = &((whitelist_t *)waf_conf->whitelist->elts)[j];

            WAF_LOG(NOTICE, cf->log, "WhileList id [%V]", &wl->id);
        }
    }

    return NGX_OK;
}

static
void * waf_load_conf_cc(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf)
{
    cJSON                *cc_root, *sub_item, *item = NULL;
    ngx_int_t            rule_cnt = 0, i, rc;
    cc_custom_policy_t   *policy;
    value_handler_t      thresh_handler;

    cc_root = waf_get_node(root, "AntiCC", NULL);
    if (cc_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no AntiCC cmd in waf.dat...skip the function");
        return NGX_OK;
    }

    rc = waf_get_cmd(cc_root, waf_switch_str_to_enum, cf->pool, 
                     &waf_conf->cc_enabled, "AntiCCEngine", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(cc_root, waf_value_ngx_str_copy, cf->pool,
                       &waf_conf->cc_var, "AntiCCVariable", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(cc_root, waf_parse_freq_base_threshold, cf->pool,
                       &waf_conf->cc_threshold, "AntiCCThreshold", NULL);
    if (rc != NGX_OK) {
        WAF_LOG(ERR, cf->log, "no waf cmd in func [%s]", __func__);
        return NGX_CONF_ERROR;
    }
    waf_conf->cc_node_expired = waf_conf->cc_threshold.freq_base_interval;

    rc = waf_get_cmd(cc_root, waf_action_str_to_enum, cf->pool, 
                       &waf_conf->cc_action, "AntiCCAction", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(cc_root, waf_value_time_str_to_sec_int, cf->pool,
                      &waf_conf->cc_action_time, "AntiCCActionExpired", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    WAF_LOG(NOTICE, cf->log, "AntiCCEngine: [%d]",       
                     waf_conf->cc_enabled);
    WAF_LOG(NOTICE, cf->log, "AntiCCVariable: [%V]",      
                     &waf_conf->cc_var);
    WAF_LOG(NOTICE, cf->log, "AntiCCThreshold: [%d/%d]",    
                     waf_conf->cc_threshold.freq_base_times, 
                     waf_conf->cc_threshold.freq_base_interval);
    WAF_LOG(NOTICE, cf->log, "AntiCCAction: [%d]",        
                     waf_conf->cc_action);
    WAF_LOG(NOTICE, cf->log, "AntiCCActionExpired: [%d]",
                     waf_conf->cc_action_time);
    WAF_LOG(NOTICE, cf->log, "AntiCCNodeExpired: [%d]",   
                     waf_conf->cc_node_expired);

    rule_cnt = waf_get_array_size(cc_root, "AntiCCCustomPolicy", NULL);
    if (rule_cnt > 0) {

        item = waf_get_node(cc_root, "AntiCCCustomPolicy", NULL);
    }

    /* 后面waf_ngx_str_variable_split中要用到app_cc, 所以提前注册app */
    if (waf_conf->cc_enabled) {

        if (waf_comm_hash_app_register(cf, waf_conf, waf_comm_cc_app_id, 
                                    &waf_conf->cc_var,
                                    WAF_STATS_METHOD_ACCESS,
                                    &waf_conf->cc_threshold, 
                                    waf_conf->cc_action, 
                                    waf_conf->cc_action_time,
                                    waf_conf->cc_node_expired,
                                    &app_cc) != NGX_OK) {
            return NGX_CONF_ERROR;
        }
    } else if (rule_cnt > 0) {

        /* CC默认策略关闭，仅开启自定义策略 */
        if (waf_comm_hash_app_register(cf, waf_conf, waf_comm_cc_app_id, 
                                    NULL,
                                    WAF_STATS_METHOD_ACCESS,
                                    NULL, 
                                    WAF_ACT_NONE, 
                                    0,
                                    waf_conf->cc_node_expired,
                                    &app_cc) != NGX_OK) {
            return NGX_CONF_ERROR;
        }
    }

    for (i = 0; i < rule_cnt; i++) {        

        policy = (cc_custom_policy_t *)ngx_array_push(waf_conf->cc_policies);
        if (policy == NULL) {
            return NGX_CONF_ERROR;
        }

        ngx_memzero(policy, sizeof(cc_custom_policy_t));

        policy->var_comb = (waf_variable_comb_t *)ngx_pcalloc(cf->pool,
                                       sizeof(waf_variable_comb_t));
        if (policy->var_comb == NULL) {
            return NGX_CONF_ERROR;
        }

        sub_item  = cJSON_GetArrayItem(item, i);

        rc = waf_get_cmd(sub_item, waf_value_ngx_str_copy, cf->pool, 
                               &policy->policy_name, "name", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(sub_item, waf_value_str_to_int, cf->pool, 
                               &policy->match_id, "matchid", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(sub_item, waf_cc_policy_mode_to_enum, cf->pool,
                         &policy->mode, "mode", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }
        
        rc = waf_get_cmd(sub_item, waf_value_ngx_str_copy, cf->pool, 
                         &policy->match_method, "matchmethod", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        waf_cf_metadata_t meta;

        ngx_memzero(&meta, sizeof(meta));
        meta.cf        = cf;
        meta.var_comb  = policy->var_comb;

        if (policy->mode == CC_POLICY_MODE_FREQUENCY) {

            rc = waf_get_cmd(sub_item, waf_ngx_str_variable_split, cf->pool,
                                    &meta, "variable", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }
        }

        if (policy->mode == CC_POLICY_MODE_FREQUENCY) {
            thresh_handler = waf_parse_freq_base_threshold;
        } else if (policy->mode == CC_POLICY_MODE_RESP_TIME) {
            thresh_handler = waf_parse_time_base_threshold;
        } else {
            WAF_LOG(ERR, cf->log, "Invalid CC policy mode");
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(sub_item, thresh_handler, cf->pool,
                         &policy->threshold, "threshold", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }
        policy->node_expired = policy->threshold.freq_base_interval;

        rc = waf_get_cmd(sub_item, waf_action_str_to_enum, cf->pool, 
                         &policy->action, "action", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(sub_item, waf_value_time_str_to_sec_int, cf->pool,
                         &policy->action_time, "time", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        WAF_LOG(NOTICE, cf->log, "policy_id: [%V]",     &policy->policy_name);
        WAF_LOG(NOTICE, cf->log, "matchid: [%d]",       (int)policy->match_id);
        WAF_LOG(NOTICE, cf->log, "mode: [%d]",          policy->mode);
        WAF_LOG(NOTICE, cf->log, "matchmethod: [%V]",   &policy->match_method);            
        WAF_LOG(NOTICE, cf->log, "variable: [%V]",      &policy->var_comb->var);             
        WAF_LOG(NOTICE, cf->log, "nodeexpired: [%d]",   policy->node_expired);

        if (policy->mode == CC_POLICY_MODE_FREQUENCY) {
            WAF_LOG(NOTICE, cf->log, "threshold: [%d/%d]",  
                policy->threshold.freq_base_times, 
                policy->threshold.freq_base_interval); 
        } else if (policy->mode == CC_POLICY_MODE_RESP_TIME) {
            WAF_LOG(NOTICE, cf->log, "threshold: [%dms]",  
                policy->threshold.time_base_ms);
        } 
        
        WAF_LOG(NOTICE, cf->log, "action: [%d]",        policy->action);             
        WAF_LOG(NOTICE, cf->log, "action_time: [%d]",   policy->action_time);
    }

    ngx_str_t resp_time_name = ngx_string("upstream_response_time");
    resp_time_var_index = ngx_http_get_variable_index(cf, &resp_time_name);
    if (resp_time_var_index < 0) {
        return NGX_CONF_ERROR;
    }

    return NGX_OK;
}

static
void * waf_load_conf_spider(ngx_conf_t *cf, cJSON *root, 
                            waf_main_conf_t *waf_conf)
{
    cJSON                *sd_root;
    ngx_int_t            rc;

    sd_root = waf_get_node(root, "AntiSD", NULL);
    if (sd_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no AntiSD cmd in waf.dat...skip the function");
        return NGX_OK;
    }

    rc = waf_get_cmd(sd_root, waf_switch_str_to_enum, cf->pool,
                     &waf_conf->sd_enabled, "AntiSDEngine", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(sd_root, waf_value_ngx_str_copy, cf->pool, 
                     &waf_conf->sd_var, "AntiSDVariable", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(sd_root, waf_parse_freq_base_threshold, cf->pool,
                     &waf_conf->sd_threshold, "AntiSDThreshold", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }
    waf_conf->sd_node_expired = waf_conf->sd_threshold.freq_base_interval;

    rc = waf_get_cmd(sd_root, waf_action_str_to_enum, cf->pool, 
                     &waf_conf->sd_action, "AntiSDAction", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(sd_root, waf_value_time_str_to_sec_int, cf->pool, 
                     &waf_conf->sd_action_time, "AntiSDActionExpired", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    WAF_LOG(NOTICE, cf->log, "AntiSDEngine: [%d]",        waf_conf->sd_enabled);
    WAF_LOG(NOTICE, cf->log, "AntiSDVariable: [%V]",      &waf_conf->sd_var);
    WAF_LOG(NOTICE, cf->log, "AntiSDThreshold: [%d/%d]",  
                              waf_conf->sd_threshold.freq_base_times,
                              waf_conf->sd_threshold.freq_base_interval);
    WAF_LOG(NOTICE, cf->log, "AntiSDAction: [%d]",        waf_conf->sd_action);
    WAF_LOG(NOTICE, cf->log, "AntiSDActionExpired: [%d]", waf_conf->sd_action_time);
    WAF_LOG(NOTICE, cf->log, "AntiSDNodeExpired: [%d]",   waf_conf->sd_node_expired);

    if (waf_conf->sd_enabled) {
        if (waf_comm_hash_app_register(cf, waf_conf, waf_comm_antisd_app_id,
                         &waf_conf->sd_var,
                        WAF_STATS_METHOD_ACCESS,
                         &waf_conf->sd_threshold, 
                         waf_conf->sd_action,
                         waf_conf->sd_action_time,
                         waf_conf->sd_node_expired,
                         NULL) != NGX_OK) {
            return NGX_CONF_ERROR;
        }
    }

    return NGX_OK;
}

static
void * waf_load_conf_bruteforce(ngx_conf_t *cf, cJSON *root, 
                            waf_main_conf_t *waf_conf)
{
    cJSON          *bf_root;
    ngx_int_t      rc;

    bf_root = waf_get_node(root, "AntiBF", NULL);
    if (bf_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no AntiBF cmd in waf.dat...skip the function");
        return NGX_OK;
    }

    rc = waf_get_cmd(bf_root, waf_switch_str_to_enum, cf->pool, 
                     &waf_conf->bf_enabled, "AntiBFEngine", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(bf_root, waf_value_ngx_str_copy, cf->pool, 
                     &waf_conf->bf_var, "AntiBFVariable", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(bf_root, waf_parse_freq_base_threshold, cf->pool, 
                     &waf_conf->bf_threshold, "AntiBFThreshold", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }
    waf_conf->bf_node_expired = waf_conf->bf_threshold.freq_base_interval;

    rc = waf_get_cmd(bf_root, waf_action_str_to_enum, cf->pool, 
                     &waf_conf->bf_action, "AntiBFAction", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(bf_root, waf_value_time_str_to_sec_int, cf->pool, 
                     &waf_conf->bf_action_time, "AntiBFActionExpired", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    WAF_LOG(NOTICE, cf->log, "AntiBFEngine: [%d]",        waf_conf->bf_enabled);
    WAF_LOG(NOTICE, cf->log, "AntiBFVariable: [%V]",      &waf_conf->bf_var);
    WAF_LOG(NOTICE, cf->log, "AntiBFThreshold: [%d/%d]",     
                              waf_conf->bf_threshold.freq_base_times,
                              waf_conf->bf_threshold.freq_base_interval);
    WAF_LOG(NOTICE, cf->log, "AntiBFAction: [%d]",        waf_conf->bf_action);
    WAF_LOG(NOTICE, cf->log, "AntiBFActionExpired: [%d]", waf_conf->bf_action_time);
    WAF_LOG(NOTICE, cf->log, "AntiBFNodeExpired: [%d]",   waf_conf->bf_node_expired);

    if (waf_conf->bf_enabled) {

        if (waf_comm_hash_app_register(cf, waf_conf, waf_comm_antibf_app_id,
                         &waf_conf->bf_var,
                        WAF_STATS_METHOD_LOGIN,
                         &waf_conf->bf_threshold, 
                         waf_conf->bf_action,
                         waf_conf->bf_action_time,
                         waf_conf->bf_node_expired,
                         NULL) != NGX_OK) {
            return NGX_CONF_ERROR;
        }
    }

    return NGX_OK;
}

static
void * waf_load_conf_bot(ngx_conf_t *cf, cJSON *root, 
                            waf_main_conf_t *waf_conf)
{
    cJSON          *bot_root, *item = NULL, *sub_item;
    ngx_int_t      rc, rule_cnt, i;
    bot_policy_t   *policy;

    bot_root = waf_get_node(root, "AntiBOT", NULL);
    if (bot_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no AntiBOT cmd in waf.dat...skip the function");
        return NGX_OK;
    }

    rc = waf_get_cmd(bot_root, waf_switch_str_to_enum, cf->pool, 
                     &waf_conf->bot_enabled, "AntiBOTEngine", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rule_cnt = waf_get_array_size(bot_root, "AntiBOTCustomPolicy", NULL);
    if (rule_cnt > 0) {

        item = waf_get_node(bot_root, "AntiBOTCustomPolicy", NULL);
    }

    WAF_LOG(NOTICE, cf->log, "AntiBOTEngine: [%d]",        waf_conf->bot_enabled);

    if (waf_conf->bot_enabled && rule_cnt > 0) {

        waf_threshold_t threshold;

        waf_conf->bot_node_expired = 0;

        for (i = 0; i < rule_cnt; i++) {        

            sub_item  = cJSON_GetArrayItem(item, i);

            rc = waf_get_cmd(sub_item, waf_parse_freq_base_threshold, cf->pool,
                             &threshold, "threshold", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            /* 用所有规则中的最大超时时间作为BOT节点的超时时间 */
            if (threshold.freq_base_interval > (ngx_uint_t)waf_conf->bot_node_expired) {
                waf_conf->bot_node_expired = threshold.freq_base_interval;
            }
        }

        WAF_LOG(NOTICE, cf->log, "AntiBOTNodeExpired: [%d]",  
                     waf_conf->bot_node_expired);

        if (waf_comm_hash_app_register(cf, waf_conf, waf_comm_antibot_app_id, 
                                    NULL,
                                    WAF_STATS_METHOD_ACCESS,
                                    NULL, 
                                    WAF_ACT_NONE, 
                                    0,
                                    waf_conf->bot_node_expired,
                                    &app_bot) != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        for (i = 0; i < rule_cnt; i++) {        

            policy = (bot_policy_t *)ngx_array_push(waf_conf->bot_policies);
            if (policy == NULL) {
                return NGX_CONF_ERROR;
            }

            ngx_memzero(policy, sizeof(bot_policy_t));

            sub_item  = cJSON_GetArrayItem(item, i);

            rc = waf_get_cmd(sub_item, waf_value_ngx_str_copy, cf->pool, 
                                   &policy->policy_name, "name", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            rc = waf_get_cmd(sub_item, waf_value_str_to_int, cf->pool, 
                                   &policy->match_id, "matchid", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            rc = waf_get_cmd(sub_item, waf_parse_freq_base_threshold, cf->pool,
                             &policy->threshold, "threshold", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            rc = waf_get_cmd(sub_item, waf_action_str_to_enum, cf->pool, 
                             &policy->action, "action", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            rc = waf_get_cmd(sub_item, waf_value_time_str_to_sec_int, cf->pool,
                             &policy->action_time, "time", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            WAF_LOG(NOTICE, cf->log, "policy_name: [%V]",     &policy->policy_name);
            WAF_LOG(NOTICE, cf->log, "matchid: [%d]",  (int)policy->match_id);     
            WAF_LOG(NOTICE, cf->log, "threshold: [%d/%d]",  
                            policy->threshold.freq_base_times, 
                            policy->threshold.freq_base_interval);         
            WAF_LOG(NOTICE, cf->log, "action: [%d]",        policy->action);
            WAF_LOG(NOTICE, cf->log, "action_time: [%d]",   policy->action_time);
        }

    }



    return NGX_OK;
}

static
void * waf_load_conf_dlp(ngx_conf_t *cf, cJSON *root, 
                            waf_main_conf_t *waf_conf)
{
    ngx_int_t         rc, j;
    cJSON             *dlp_root, *item, *sub_item;
    ngx_uint_t        policy_cnt, i;
    dlp_policy_t       *policy;

    dlp_root = waf_get_node(root, "DLP", NULL);
    if (dlp_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no DLP cmd in waf.dat...skip the function");
        return NGX_OK;
    }

    policy_cnt = waf_get_array_size(dlp_root, "DLPPolicy", NULL);
    if (policy_cnt > 0) {

        item = waf_get_node(dlp_root, "DLPPolicy", NULL);

        if (item == NULL) {
            return NGX_CONF_ERROR;
        }

        for (i = 0; i < policy_cnt; i++) {
            sub_item  = cJSON_GetArrayItem(item, i);
            if (sub_item == NULL) {
                return NGX_CONF_ERROR;
            }

            policy = (dlp_policy_t *)ngx_array_push(waf_conf->dlp_policies);

            if (policy == NULL) {
                return NGX_CONF_ERROR;
            }

            ngx_memzero(policy, sizeof(dlp_policy_t));

            rc = waf_get_cmd(sub_item, waf_value_ngx_str_copy, cf->pool,
                             &policy->policy_id, 
                             "name", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            rc = waf_get_cmd(sub_item, waf_parse_dlp_mode, cf->pool,
                             &policy->mode, 
                             "mode", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            if (policy->mode == DLP_MODE_CONTENT) {
                rc = waf_get_cmd(sub_item, waf_value_str_to_int, cf->pool,
                                 &policy->match_id, 
                                 "matchid", NULL);
                if (rc != NGX_OK) {
                    return NGX_CONF_ERROR;
                }

                rc = waf_get_cmd(sub_item, waf_value_str_to_int, cf->pool,
                                 &policy->threshold, 
                                 "threshold", NULL);
                if (rc != NGX_OK) {
                    return NGX_CONF_ERROR;
                }

                rc = waf_get_cmd(sub_item, waf_action_str_to_enum, cf->pool,
                                 &policy->action, 
                                 "action", NULL);
                if (rc != NGX_OK) {
                    return NGX_CONF_ERROR;
                }

            } else if (policy->mode == DLP_MODE_ERRCODE) {

                rc = waf_get_cmd(sub_item, waf_rule_id_split, cf->pool,
                                 &policy->errorcode, 
                                 "errorcode", NULL);
                if (rc != NGX_OK) {
                    return NGX_CONF_ERROR;
                }

                rc = waf_get_cmd(sub_item, waf_action_str_to_enum, cf->pool,
                                 &policy->action, 
                                 "action", NULL);
                if (rc != NGX_OK) {
                    return NGX_CONF_ERROR;
                }

            }
        }
    }

    for (j = 0; policy_cnt > 0 && j < (int)waf_conf->dlp_policies->nelts; j++) {
        policy = &((dlp_policy_t *)waf_conf->dlp_policies->elts)[j];

        WAF_LOG(NOTICE, cf->log, "DLP policy_id: [%V]",  &policy->policy_id); 
        WAF_LOG(NOTICE, cf->log, "DLP mode     : [%d]",  policy->mode);

        if (policy->mode == DLP_MODE_CONTENT) {
            WAF_LOG(NOTICE, cf->log, "DLP matchid: [%d]", (int)policy->match_id);
            WAF_LOG(NOTICE, cf->log, "DLP threshold: [%d]", policy->threshold); 
            WAF_LOG(NOTICE, cf->log, "DLP action   : [%d]", policy->action); 

        } else if (policy->mode == DLP_MODE_ERRCODE) {

            WAF_LOG(NOTICE, cf->log, "DLP action   : [%d]", policy->action); 

            for (i = 0; i < (ngx_uint_t)policy->errorcode->nelts; i++) {
                ngx_str_t *errcode = &((ngx_str_t *)policy->errorcode->elts)[i];
                WAF_LOG(NOTICE, cf->log, "DLP errorcode: [%V]", errcode);      
            }
        }
    }

    return NGX_OK;
}

static
void * waf_load_conf_weblock(ngx_conf_t *cf, cJSON *root, 
                            waf_main_conf_t *waf_conf)
{
    cJSON             *weblock_root = NULL, 
                      *rule_root    = NULL, 
                      *rule_item;
    ngx_int_t         rule_cnt, i, rc;
    web_lock_entry_t  *rule;

    weblock_root = waf_get_node(root, "WebLock", NULL);
    if (weblock_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no WebLock cmd in waf.dat..."
                        "skip the function");
        return NGX_OK;
    }

    rc = waf_get_cmd(weblock_root, waf_value_file_size_str_to_byte_int, cf->pool,
                     &waf_conf->weblock_item_size, "WebLockItemSize", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(weblock_root, waf_value_file_size_str_to_byte_int, cf->pool,
                     &waf_conf->weblock_disk_size_max, "WebLockDiskSizeMax", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    WAF_LOG(NOTICE, cf->log, "WebLockItemSize: [%d] bytes",    
             waf_conf->weblock_item_size);
    WAF_LOG(NOTICE, cf->log, "WebLockDiskSizeMax: [%d] bytes",  
             waf_conf->weblock_disk_size_max);

    rule_cnt = waf_get_array_size(weblock_root, "WebLockRule", NULL);
    if (rule_cnt > 0) {

        rule_root = waf_get_node(weblock_root, "WebLockRule", NULL);
    }

    for (i = 0; i < rule_cnt; i++) {        

        rule = (web_lock_entry_t *)ngx_array_push(waf_conf->weblock_array);
        if (rule == NULL) {
            WAF_LOG(ERR, cf->log, "alloc in func [%s] failed", __func__);
            return NGX_CONF_ERROR;
        }

        ngx_memzero(rule, sizeof(web_lock_entry_t));

        rule_item  = cJSON_GetArrayItem(rule_root, i);

        rc = waf_get_cmd(rule_item, waf_value_ngx_str_copy, cf->pool, 
                         &rule->id, "id", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(rule_item, waf_switch_str_to_enum, cf->pool, 
                         &rule->is_enabled, "switch", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(rule_item, waf_value_str_to_int, cf->pool, 
                         &rule->last_update, "lastupdate", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(rule_item, waf_value_ngx_str_copy, cf->pool, 
                         &rule->url, "hosturl", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(rule_item, waf_value_str_to_int, cf->pool, 
                         &rule->match_id, "matchid", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        if (waf_weblock_gen_escaped_url(cf->pool, rule) != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        WAF_LOG(NOTICE, cf->log, "WeblockRule id: [%V]",          &rule->id);
        WAF_LOG(NOTICE, cf->log, "WeblockRule is enabled: [%d]",  rule->is_enabled);
        WAF_LOG(NOTICE, cf->log, "WeblockRule interval update: [%d]", rule->last_update);
        WAF_LOG(NOTICE, cf->log, "WeblockRule hosturl: [%V]",     &rule->url);
        WAF_LOG(NOTICE, cf->log, "WeblockRule matchid: [%d]",     rule->match_id);
    }

    if (waf_weblock_disk_init(cf, waf_conf) != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    return NGX_OK;
}

#if 0
static
void * waf_load_conf_internal_rules(ngx_conf_t *cf, cJSON *root, 
                            waf_main_conf_t *waf_conf)
{
    cJSON         *interrule_root, *rules, *rule_item = NULL;
    ngx_int_t     rule_cnt, i, rc;
    waf_rule_t    *rule;

    if ((waf_conf->intr_handler = new wafMatch_t) == NULL) {
        WAF_LOG(ERR, cf->log, "new intr match handler return NULL.");
        return NGX_CONF_ERROR;
    }

    (waf_conf->intr_handler)->is_intr_rule = 1;

    interrule_root = waf_get_node(root, "InternalRule", NULL);
    if (interrule_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no InternalRule cmd in waf.dat..."
                        "skip the function");
        return NGX_OK;
    }

    rule_cnt = waf_get_array_size(interrule_root, "Rules", NULL);
    if (rule_cnt <= 0) {
        WAF_LOG(ERR, cf->log, "no rules in InternalRule");
        return NGX_CONF_ERROR;
    }

    rules = waf_get_node(interrule_root, "Rules", NULL);
    if (rules == NULL) {
        WAF_LOG(ERR, cf->log, "get rules in internal rule failed");
        return NGX_CONF_ERROR;
    }

    for (i = 0; i < rule_cnt; i++) {

        rule = (waf_rule_t *)ngx_array_push(waf_conf->intr_rules);
        if (rule == NULL) {
            WAF_LOG(ERR, cf->log, "alloc in func [%s] failed", __func__);
            return NGX_CONF_ERROR;
        }

        ngx_memzero(rule, sizeof(waf_rule_t));

        rule_item  = cJSON_GetArrayItem(rules, i);

        rc = waf_get_cmd(rule_item, waf_value_ngx_str_copy, cf->pool, 
                         &rule->id, "id", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        /* 第一次解析mz字符串整体 */
        rc = waf_get_cmd(rule_item, waf_value_ngx_str_copy, cf->pool, 
                         &rule->mz, "mz", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        /* 第二次将mz中的各变量解析出来 */
        rc = waf_get_cmd(rule_item, waf_ngx_str_variable_split_dot, cf->pool, 
                         (void *)&rule->mz_array, "mz", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(rule_item, waf_value_ngx_str_copy, cf->pool, 
                         &rule->rx, "rx", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(rule_item, waf_value_logic_to_enum, cf->pool, 
                         &rule->logic, "logic", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }
        
        WAF_LOG(NOTICE, cf->log, "Internal rule id:    [%V]",  &rule->id);
        WAF_LOG(NOTICE, cf->log, "Internal rule mz:    [%V]",  &rule->mz);
        WAF_LOG(NOTICE, cf->log, "Internal rule rx:    [%V]",  &rule->rx);
        WAF_LOG(NOTICE, cf->log, "Internal rule logic: [%d]",  rule->logic);

        if (waf_load_rule(cf, waf_conf, waf_conf->intr_handler, rule, 
                            INTER_RULES_HS_FLAGS) != NGX_OK) {
            WAF_LOG(ERR, cf->log, "internal rules waf_load_rule failed");
            return NGX_CONF_ERROR;
        }
    }

    if (waf_conf->intr_handler->rules_to_hs() != 0) {
        WAF_LOG(ERR, cf->log, "internal rules to hs failed.");
        return NGX_CONF_ERROR;
    }

    return NGX_OK;
}

#endif

static
void * waf_load_conf_user_rules(ngx_conf_t *cf, cJSON *root, 
                            waf_main_conf_t *waf_conf)
{
    cJSON         *user_root, *rules, *rule_item = NULL;
    ngx_int_t     rule_cnt, i, rc;
    waf_rule_t    *rule;

    if ((waf_conf->user_handler = new wafMatch_t) == NULL) {
        WAF_LOG(ERR, cf->log, "new user match handler return NULL.");
        return NGX_CONF_ERROR;
    }

    user_root = waf_get_node(root, "UserRule", NULL);
    if (user_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no UserRule cmd in waf.dat..."
                        "skip the function");
        return NGX_OK;
    }

    rule_cnt = waf_get_array_size(user_root, "Policies", NULL);
    if (rule_cnt <= 0) {

        WAF_LOG(ERR, cf->log, "no Policies in user rule");
        return NGX_CONF_ERROR;
    }

    rules = waf_get_node(user_root, "Policies", NULL);
    if (rules == NULL) {
        WAF_LOG(ERR, cf->log, "get rules in user rule failed");
        return NGX_CONF_ERROR;
    }

    for (i = 0; i < rule_cnt; i++) {

        rule_item = cJSON_GetArrayItem(rules, i);

        rule = (waf_rule_t *)ngx_array_push(waf_conf->user_rules);
        if (rule == NULL) {

            WAF_LOG(ERR, cf->log, "alloc in func [%s] failed", __func__);
            return NGX_CONF_ERROR;
        }

        ngx_memzero(rule, sizeof(waf_rule_t));

        rc = waf_get_cmd(rule_item, waf_value_ngx_str_copy, cf->pool, 
                         &rule->id, "matchid", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(rule_item, waf_action_str_to_enum, cf->pool, 
                         &rule->action, "action", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        WAF_LOG(NOTICE, cf->log, "User rule id: [%V]",  &rule->id);
        WAF_LOG(NOTICE, cf->log, "User rule action: [%d]",  rule->action);
    }

    return NGX_OK;
}

static
void * waf_load_conf_sec_rules(ngx_conf_t *cf, cJSON *root, 
                            waf_main_conf_t *waf_conf)
{
    cJSON         *secrule_root, *rules, *rule_item = NULL;
    ngx_int_t     rule_cnt, i, rc;
    waf_rule_t    *rule;

    WAF_LOG(NOTICE, cf->log, "Load SecRule ...");

    if ((waf_conf->sec_handler = new wafMatch_t) == NULL) {
        WAF_LOG(ERR, cf->log, "new sec match handler return NULL.");
        return NGX_CONF_ERROR;
    }

    secrule_root = waf_get_node(root, "SecRule", NULL);
    if (secrule_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no SecRule cmd in waf.dat..."
                        "skip the function");
        return NGX_OK;
    }

    rule_cnt = waf_get_array_size(secrule_root, "Policies", NULL);
    
    if (rule_cnt <= 0) {
        WAF_LOG(ERR, cf->log, "no Policies in SecRule");
        return NGX_CONF_ERROR;
    }

    rules = waf_get_node(secrule_root, "Policies", NULL);
    if (rules == NULL) {
        WAF_LOG(ERR, cf->log, "get rules in sec rule failed");
        return NGX_CONF_ERROR;
    }

    for (i = 0; i < rule_cnt; i++) {

        rule = (waf_rule_t *)ngx_array_push(waf_conf->sec_rules);
        if (rule == NULL) {
            WAF_LOG(ERR, cf->log, "alloc in func [%s] failed", __func__);
            return NGX_CONF_ERROR;
        }

        ngx_memzero(rule, sizeof(waf_rule_t));

        rule_item  = cJSON_GetArrayItem(rules, i);

        rc = waf_get_cmd(rule_item, waf_value_ngx_str_copy, cf->pool, 
                         &rule->id, "matchid", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(rule_item, waf_action_str_to_enum, cf->pool, 
                         &rule->action, "action", NULL);
        if (rc != NGX_OK) {
            /* 用全局动作做为每个rule的动作 */
            rule->action = waf_conf->WafAction;
        }

        WAF_LOG(NOTICE, cf->log, "Sec rule id: [%V]",  &rule->id);
        WAF_LOG(NOTICE, cf->log, "Sec rule action: [%d]",  rule->action);
    }

    return NGX_OK;
}

static void *
waf_load_conf_waf_monitor(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf)
{
    ngx_int_t rc;
    cJSON                *sub_root;


    waf_conf->WMEngine = 0;
    sub_root = waf_get_node(root, "Monitor", NULL);
    if (sub_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no Monitor cmd in waf.dat...skip the function");
        return NGX_OK;
    }

    rc = waf_get_cmd(sub_root,  waf_switch_str_to_enum, 
            cf->pool, &waf_conf->WMEngine, "WMEngine", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(sub_root, waf_value_time_str_to_sec_int, cf->pool,
                      &waf_conf->WMOldMasterExitTime, "WMOldMasterExitTime", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(sub_root, waf_value_time_str_to_sec_int, cf->pool,
                      &waf_conf->WMMasterInterval, "WMMasterInterval", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(sub_root, waf_value_time_str_to_sec_int, cf->pool,
                      &waf_conf->WMWorkerInterval, "WMWorkerInterval", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(sub_root, waf_parse_freq_base_threshold, cf->pool,
                      &waf_conf->WMTimeThreshold, "WMTimeThreshold", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(sub_root, waf_parse_freq_base_threshold, cf->pool,
                      &waf_conf->WMCHLDThreshold, "WMCHLDThreshold", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }


    WAF_LOG(NOTICE, cf->log, "WMEngine:[%i]", waf_conf->WMEngine);
    WAF_LOG(NOTICE, cf->log, "WMOldMasterExitTime: [%i]", waf_conf->WMOldMasterExitTime);
    WAF_LOG(NOTICE, cf->log, "WMMasterInterval: [%i]", waf_conf->WMMasterInterval);
    WAF_LOG(NOTICE, cf->log, "WMWorkerInterval: [%i]", waf_conf->WMWorkerInterval);
    WAF_LOG(NOTICE, cf->log, "WMTimeThreshold: [%i/%i]", 
            waf_conf->WMTimeThreshold.freq_base_times,
            waf_conf->WMTimeThreshold.freq_base_interval);
    WAF_LOG(NOTICE, cf->log, "WMCHLDThreshold: [%i/%i]", 
            waf_conf->WMCHLDThreshold.freq_base_times,
            waf_conf->WMCHLDThreshold.freq_base_interval);

    return NGX_OK;
}

static void *
waf_load_conf_malicous_ip(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf)
{
    ngx_int_t rc;
    cJSON                *sub_root;

    waf_conf->MaliciousIPEngine = 0;
    sub_root = waf_get_node(root, "MaliciousIP", NULL);
    if (sub_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no MaliciousIP cmd in waf.dat...skip the function");
        return NGX_OK;
    }

    rc = waf_get_cmd(sub_root,  waf_switch_str_to_enum, cf->pool, 
                     &waf_conf->MaliciousIPEngine, "MaliciousIPEngine", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(sub_root, waf_parse_freq_base_threshold, cf->pool,
                      &waf_conf->MaliciousIPThreshold, "MaliciousIPThreshold", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(sub_root, waf_action_str_to_enum, cf->pool,
                      &waf_conf->MaliciousIPBlockAction, "MaliciousIPBlockAction", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    rc = waf_get_cmd(sub_root, waf_value_time_str_to_sec_int, cf->pool,
                      &waf_conf->MaliciousIPBlockTime, "MaliciousIPBlockTime", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    WAF_LOG(NOTICE, cf->log, "MaliciousIPEngine: [%i]", waf_conf->MaliciousIPEngine);
    WAF_LOG(NOTICE, cf->log, "MaliciousIPThreshold: [%i/%i]", 
            waf_conf->MaliciousIPThreshold.freq_base_times,
            waf_conf->MaliciousIPThreshold.freq_base_interval);
    WAF_LOG(NOTICE, cf->log, "MaliciousIPBlockAction: [%d]", waf_conf->MaliciousIPBlockAction);
    WAF_LOG(NOTICE, cf->log, "MaliciousIPBlockTime: [%i]", waf_conf->MaliciousIPBlockTime);

    /* load manual block rule */
    ngx_int_t i, rule_cnt = waf_get_array_size(sub_root, "MaliciousIPRule", NULL);
    MaliciousIP_rule_t *rule;
    cJSON *rule_node = NULL, *rules_node = NULL;

    if (rule_cnt > 0) {
        rules_node = waf_get_node(sub_root, "MaliciousIPRule", NULL);
    }

    for (i = 0; i < rule_cnt; i++) {        
        rule = (MaliciousIP_rule_t *)ngx_array_push(waf_conf->MaliciousIP_rule_head);
        if (rule == NULL) {
            return NGX_CONF_ERROR;
        }
        ngx_memzero(rule, sizeof(MaliciousIP_rule_t));

        /* get rule array */
        rule_node  = cJSON_GetArrayItem(rules_node, i);

        rc = waf_get_cmd(rule_node, waf_switch_str_to_enum, cf->pool,
                         &rule->enable, "switch", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(rule_node, waf_value_ngx_str_copy, 
                         cf->pool, &rule->id, "id", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(rule_node, waf_action_str_to_enum, 
                         cf->pool, &rule->action, "action", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }

        rc = waf_get_cmd(rule_node, waf_value_time_str_to_sec_int, 
                         cf->pool, &rule->block, "date", NULL);
        if (rc != NGX_OK) {
            return NGX_CONF_ERROR;
        }


        WAF_LOG(NOTICE, cf->log, "switch [%i] id:[%V] action:[%d] block:[%d]",
                     rule->enable, &rule->id, rule->action, rule->block);

        /* addrs */
        ngx_int_t addr_cnt = waf_get_array_size(rule_node, "addrs", NULL);
        cJSON *addrs_node, *addr_node;
        ngx_cidr_t *cidr;
        ngx_str_t pstr;
        ngx_int_t j;

        if (addr_cnt == 0) {
            continue;
        }

        addrs_node = waf_get_node(rule_node, "addrs", NULL);
        if (addrs_node == NULL) {
            WAF_LOG(NOTICE, cf->log, "no addrs cmd in waf.dat...skip the function");
            return NGX_OK;
        }

        rule->address_head = ngx_array_create(cf->pool, 
                MaliciousIP_RULE_ADDRS_MAX, sizeof(ngx_cidr_t));
        if (rule->address_head == NULL) {
            WAF_LOG(NOTICE, cf->log, "no addrs cmd in waf.dat...skip the function");
            return NGX_OK;
        }
        
        for (j = 0; j < addr_cnt; j++) {        
            addr_node  = cJSON_GetArrayItem(addrs_node, j);
            if (addr_node == NULL) {
                WAF_LOG(NOTICE, cf->log, "no addr node in waf.dat...skip the function");
            }

            pstr.data = (u_char *)cJSON_GetStringValue(addr_node);
            if (pstr.data == NULL) {
                continue;
            }
            pstr.len = ngx_strlen(pstr.data);

            if ((cidr = (ngx_cidr_t *)ngx_array_push(rule->address_head)) == NULL) {
                WAF_LOG(NOTICE, cf->log, "ngx_array_push cdir error.");
                return NGX_CONF_ERROR;
            }   

            ngx_memzero(cidr, sizeof(ngx_cidr_t));
            if (ngx_ptocidr(&pstr, cidr) == NGX_ERROR) {
                return NGX_CONF_ERROR;
            }   

            WAF_LOG(NOTICE, cf->log, "addr: [%V]", &pstr);
        }
    }

    return NGX_OK;
}

static void *
waf_load_conf_ip_region_rbtree(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf)
{
    ngx_int_t rc;
    cJSON                *sub_root;
    waf_ip_region_rbtree_node_t *ir_rbnode;
    cJSON *rules_node, *rule_node;
    ngx_cidr_t *cidr;

    ngx_str_t id;
    waf_action_e action;

    waf_conf->IPRegionEngine = 0;
    sub_root = waf_get_node(root, "IPRegion", NULL);
    if (sub_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no IPRegion cmd in waf.dat...skip the function");
        return NGX_OK;
    }

    rc = waf_get_cmd(sub_root,  waf_switch_str_to_enum, cf->pool, 
            &waf_conf->IPRegionEngine, "IPRegionEngine", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    WAF_LOG(NOTICE, cf->log, "IPRegionEngine:%i", waf_conf->IPRegionEngine);

    if (waf_conf->IPRegionEngine != 1) {
        WAF_LOG(NOTICE, cf->log, "IPRegion off! waf.dat...skip the function");
        return NGX_OK;
    }

    ngx_int_t i, rule_cnt = waf_get_array_size(sub_root, "IPRegionRule", NULL);

    if (rule_cnt > 0) {
        if ((rules_node = waf_get_node(sub_root, "IPRegionRule", NULL)) == NULL) {
            return NGX_OK;
        }

        for (i = 0; i < rule_cnt; i++) {
            rule_node  = cJSON_GetArrayItem(rules_node, i);
            if (rule_node == NULL) {
                break;
            }

            /* get cmd id */
            rc = waf_get_cmd(rule_node,  waf_value_ngx_str_copy, cf->pool, &id, "id", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }
            if (id.data == NULL || id.len == 0) {
                continue;
            }

            /* get cmd action */
            rc = waf_get_cmd(rule_node,  waf_action_str_to_enum, cf->pool, &action, "action", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }
            WAF_LOG(NOTICE, cf->log, "rule->id:[%V] action[%i]" , &id, action);

            if (ngx_memcmp(id.data, IPREGION_ABROAD, id.len) == 0) {
                
                /* set id */
                waf_conf->ip_region_rbtree.rule_other.id.len = id.len;
                waf_conf->ip_region_rbtree.rule_other.id.data = ngx_pstrdup(cf->pool, &id);

                /* set action */
                waf_conf->ip_region_rbtree.rule_other.action = action;
                continue;
            }

            /* get cmd addrs */
            ngx_int_t j, addr_cnt = waf_get_array_size(rule_node, "addrs", NULL);
            ngx_str_t pstr;
            cJSON *addrs_node, *addr_node;

            if (addr_cnt == 0) {
                continue;
            }

            addrs_node = waf_get_node(rule_node, "addrs", NULL);
            if (addrs_node == NULL) {
                return NGX_OK;
            }

            for(j=0; j< addr_cnt; j++) {
                addr_node  = cJSON_GetArrayItem(addrs_node, j);
                if (addr_node == NULL) {
                    break;
                }

                pstr.data = (u_char *)cJSON_GetStringValue(addr_node);
                if (pstr.data == NULL) {
                    continue;
                }
                pstr.len = ngx_strlen(pstr.data);

                ir_rbnode = (waf_ip_region_rbtree_node_t *)waf_ip_region_data_alloc(
                        cf->pool, sizeof(waf_ip_region_rbtree_node_t));
                if (ir_rbnode == NULL) {
                    return NGX_CONF_ERROR;
                }

                /* set id */
                ir_rbnode->ip_region_node.id.len = id.len;
                ir_rbnode->ip_region_node.id.data = ngx_pstrdup(cf->pool, &id);

                /* set action */
                ir_rbnode->ip_region_node.action = action;

                /* set address */
                if (ngx_ptocidr(&pstr, &ir_rbnode->ip_region_node.address) == NGX_ERROR) {
                    return NGX_CONF_ERROR;
                }

                cidr = &ir_rbnode->ip_region_node.address;

                /* set rbnode->key */
                ir_rbnode->rbnode.key = waf_ip_region_key_get(cidr);
                
                u_char *paddr, *pmask;
                paddr = (u_char *)&(cidr->u.in.addr);
                pmask = (u_char *)&(cidr->u.in.mask);
                char buff[BUFSIZ] = {0};

                snprintf((char *)buff, BUFSIZ - 1,
                        "address:[%u.%u.%u.%u] netmask:[%u.%u.%u.%u]",
                        paddr[0], paddr[1], paddr[2], paddr[3],
                        pmask[0], pmask[1], pmask[2], pmask[3]);

                WAF_LOG(DEBUG, cf->log, "address:[%s] lookupkey:[%i]", buff, ir_rbnode->rbnode.key);

                /* insert rbtree node */
                waf_ip_region_rbtree_insert(&waf_conf->ip_region_rbtree, ir_rbnode);

                //waf_ip_region_rbtree_node_show(ir_rbnode, cf->log);
            }
        }
    }

    return NGX_OK;
}

__attribute__((unused))
static void *
waf_load_conf_ip_region(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf)
{
    ngx_int_t rc;
    cJSON                *sub_root;
    IPRegion_rule_t *rule;
    cJSON *rules_node, *rule_node;

    waf_conf->IPRegionEngine = 0;
    sub_root = waf_get_node(root, "IPRegion", NULL);
    if (sub_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no IPRegion cmd in waf.dat...skip the function");
        return NGX_OK;
    }

    rc = waf_get_cmd(sub_root,  waf_switch_str_to_enum, cf->pool, 
            &waf_conf->IPRegionEngine, "IPRegionEngine", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    WAF_LOG(NOTICE, cf->log, "IPRegionEngine:%i", waf_conf->IPRegionEngine);

    ngx_int_t i, rule_cnt = waf_get_array_size(sub_root, "IPRegionRule", NULL);

    if (rule_cnt > 0) {
        if ((rules_node = waf_get_node(sub_root, "IPRegionRule", NULL)) == NULL) {
            return NGX_OK;
        }

        for (i = 0; i < rule_cnt; i++) {
            rule_node  = cJSON_GetArrayItem(rules_node, i);
            if (rule_node == NULL) {
                break;
            }

            rule = (IPRegion_rule_t *)ngx_array_push(waf_conf->IPRegion_rule_head);
            if (rule == NULL) {
                return NGX_CONF_ERROR;
            }
            ngx_memset(rule, 0, sizeof(*rule));

            /* get cmd id */
            rc = waf_get_cmd(rule_node,  waf_value_ngx_str_copy, cf->pool,
                             &rule->id, "id", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            /* get cmd action */
            rc = waf_get_cmd(rule_node,  waf_action_str_to_enum, cf->pool,
                             &rule->action, "action", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }
            WAF_LOG(NOTICE, cf->log, "rule->id:[%V] action[%i]"
                , &rule->id, rule->action);

            /* get cmd addrs */
            ngx_int_t j, addr_cnt = waf_get_array_size(rule_node, "addrs", NULL);
            ngx_cidr_t *cidr;
            ngx_str_t pstr;
            cJSON *addrs_node, *addr_node;

            if (addr_cnt == 0) {
                continue;
            }

            addrs_node = waf_get_node(rule_node, "addrs", NULL);
            if (addrs_node == NULL) {
                return NGX_OK;
            }

            rule->address_head = ngx_array_create(
                    cf->pool, addr_cnt, sizeof(ngx_cidr_t));
            if (rule->address_head == NULL) {
                return NGX_CONF_ERROR;
            }

            for(j=0; j< addr_cnt; j++) {
                addr_node  = cJSON_GetArrayItem(addrs_node, j);
                if (addr_node == NULL) {
                    break;
                }

                pstr.data = (u_char *)cJSON_GetStringValue(addr_node);
                if (pstr.data == NULL) {
                    continue;
                }
                pstr.len = ngx_strlen(pstr.data);

                if ((cidr = (ngx_cidr_t *)ngx_array_push(rule->address_head)) == NULL) {
                    return NGX_CONF_ERROR;
                }

                ngx_memset(cidr, 0, sizeof(*cidr));
                if (ngx_ptocidr(&pstr, cidr) == NGX_ERROR) {
                    return NGX_CONF_ERROR;
                }
                WAF_LOG(DEBUG, cf->log, "addr:[%V]", &pstr);
            }
        }
    }

    return NGX_OK;
}

static void *
waf_load_conf_rfc2616(ngx_conf_t *cf, cJSON *root, waf_main_conf_t *waf_conf)
{
    ngx_int_t rc;
    cJSON                *sub_root;
    rfc2616_rule_t *rule;

    waf_conf->rfc2616Engine = 0;
    sub_root = waf_get_node(root, "rfc2616", NULL);
    if (sub_root == NULL) {
        WAF_LOG(NOTICE, cf->log, "no rfc2616 cmd in waf.dat...skip the function");
        return NGX_OK;
    }

    rc = waf_get_cmd(sub_root,  waf_switch_str_to_enum, cf->pool, 
            &waf_conf->rfc2616Engine, "rfc2616Engine", NULL);
    if (rc != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    WAF_LOG(NOTICE, cf->log, "rfc2616Engine:%i", waf_conf->rfc2616Engine);

    cJSON *rules_node, *rule_node;
    ngx_int_t i, rule_cnt = waf_get_array_size(sub_root, "Rules", NULL);
    if (rule_cnt > 0) {
        if ((rules_node = waf_get_node(sub_root, "Rules", NULL)) == NULL) {
            return NGX_OK;
        }

        for (i = 0; i<rule_cnt;i++) {
            rule_node  = cJSON_GetArrayItem(rules_node, i);
            if (rule_node == NULL) {
                break;
            }

            rule = (rfc2616_rule_t *)ngx_array_push(waf_conf->rfc2616_rule_head);
            ngx_memset(rule, 0, sizeof(*rule));
            if (rule == NULL) {
                return NGX_CONF_ERROR;
            }

            /* get cmd id */
            rc = waf_get_cmd(rule_node,  waf_value_ngx_str_copy, cf->pool, &rule->id, "id", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            /* get cmd enable */
            rc = waf_get_cmd(rule_node,  waf_switch_str_to_enum, cf->pool, &rule->enable, "enable", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            /* get cmd action */
            rc = waf_get_cmd(rule_node,  waf_action_str_to_enum, cf->pool, &rule->action, "action", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }
      
            /* get type */
            rc = waf_get_cmd(rule_node,  waf_value_ngx_str_copy, cf->pool, &rule->type_str, "type", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            /* get cmd key */
            rc = waf_get_cmd(rule_node,  waf_value_ngx_str_copy, cf->pool, &rule->key_str, "key", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }

            /* get cmd value */
            rc = waf_get_cmd(rule_node,  waf_value_ngx_str_copy, cf->pool, &rule->value_str, "value", NULL);
            if (rc != NGX_OK) {
                return NGX_CONF_ERROR;
            }
        }

        if (ngx_http_waf_rfc2616_header_hash_init(cf, waf_conf) != NGX_OK) {
            WAF_LOG(ERR, cf->log, "ngx_http_waf_rfc2616_header_hash_init error!");
            return NGX_CONF_ERROR;
        }
        ngx_http_waf_rfc2616_show(cf, waf_conf);
    }

    return NGX_OK;
}


/* ------------------------对内接口结束---------------------------------------- */
