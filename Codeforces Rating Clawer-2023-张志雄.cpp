//在curl.h的头文件中修改CURL_MAX_WRITE_SIZE，调大一些
//in the curl.h header file : CURL_MAX_WRITE_SIZE(the usual default is 16K)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <time.h>
//#include <fastcgi/fcgi_stdio.h>

//读取post
//char* fun_post(void);

//输入handle并包装成api
char* input_handle(char* handle);

//使用curl库和url获取json
void get_json(char* url);

//使用curl库必须设置的回调函数
size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);

//读入数据基本信息
void basic_json(char* buf);

//解析json，把数据载入内存
void parse_json(char* handle);

//格式化字符串，把json中题目标签两端多余的引号删去
char* string_format(char* root);

//创建解析后的json
void create_json(char* handle);


//统计题目数据
struct prob                         
{
    time_t time_unix;
    struct tm time;
    //难度分布
    enum level_diff level;         
};

//统计题目标签及数目
struct prob_cat             
{
    //标签
    char* tag;              
    //tag计数
    int num_tag = 0;        
};

//统计每年做题数据
struct statistic
{
    bool leapyear;
    int year;
    int month[12];
    int month_day[12][31];
};

//定义题目评级
enum level_diff
{
    Easy = 1, Intermediate, Moderate, Challenging, Difficult, Expert
};

//读入json
cJSON* root = nullptr;      
//指向json-result
cJSON* result = nullptr;    
//指向json-status
cJSON* status = nullptr;
//总提交数
int num_all_submit = 0;                                 
//printf("num all submit = %d\n", num_all_submit);

//总通过数
int num_all_ok = 0; 


//指向json-result内的数组
cJSON* array_in_result = nullptr;
//指向json-result数组-verdict
cJSON* verdict_in_array = nullptr;
//指向json-result数组-problem
cJSON* prob_in_array = nullptr;
//指向json-result数组-problem数组-tags
cJSON* tag_in_array = nullptr;
//储存标签内容
cJSON* tags = nullptr;
//不同标签数
int tag_size = 0;
//指向json-result数组-problem数组-rating
cJSON* rating_in_array = nullptr;

//指向json-result数组-problem数组-creationtimeseconds
time_t time_temp;
cJSON* time_in_array = nullptr;                 

//指向最早的做题时间
time_t smallest_time = time(NULL);
struct tm smallest_time_info;

//指向最晚的做题时间
time_t biggest_time = time(NULL);
struct tm biggest_time_info;

//最多载入9999条数据
struct prob probs[9999];
//最多载入50种标签
struct prob_cat probs_cat[50];

int biggest_year = 0;
int smallest_year = 0;

struct statistic* statistics = nullptr;

//难度统计
int easy = 0;
int intermediate = 0;
int moderate = 0;
int challenging = 0;
int difficult = 0;
int expert = 0;

int main()
{
    printf("请输入用户名：");
    //输入用户名
    char* handle = (char*)malloc(25 * sizeof(char));

    //将用户名转化为url
    char* get_url = input_handle(handle);
    //printf("test url : %s\n", get_url);
    
    //使用curl库获取做题数据
    get_json(get_url);

    printf("成功获取用户对应数据，请见目录内response.json");

    //读入json
    FILE* fp = fopen("response.json", "r");
    if (fp == nullptr) return -1;

    fseek(fp, 0, SEEK_END);
    int filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buf = (char*)malloc(filesize * sizeof(char));
    memset(buf, 0, filesize * sizeof(char));
    if (int ret = fread(buf, filesize, sizeof(char), fp) == -1)
    {
        fclose(fp);
        return -1;
    }

    //解析json
    basic_json(buf);
    parse_json(handle);
    create_json(handle);

    //清理
    cJSON_Delete(root);
    fclose(fp);

    printf("成功解析用户数据，请见目录内response_parsed.json");

	return 0;
}

char* input_handle(char* handle)
{
    //使用curl库进行url编码
    CURL* curl = curl_easy_init();

	char* url = (char*)malloc(75 * sizeof(char));

    strcpy(url, "https://codeforces.com/api/user.status?handle=");

    //输入handle
	scanf("%s", handle);


	strcat(url, handle);

    //curl库url编码
    char* encodedUrl = curl_easy_escape(curl, url, 0);
    if (encodedUrl) {
        //printf("Encoded URL: %s\n", encodedUrl);
        curl_free(encodedUrl);
    }
	return url;
}
size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    // 创建文件句柄
    FILE* file = (FILE*)userdata;
    //判断是否创建成功
    if (file) {
        //写入文件
        fwrite(ptr, size, nmemb, file);
    }
    return size * nmemb;
}
void get_json(char* url)     //存储response.json
{
    // 初始化CURL库
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // 创建CURL句柄
    CURL* curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL\n");
        return;
    }

    // 设置请求的URL
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // 创建用于保存响应数据的文件
    FILE* file = fopen("response.json", "w");
    if (!file) {
        fprintf(stderr, "Failed to create file\n");
        curl_easy_cleanup(curl);
        return;
    }

    // 设置写入响应数据的回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    // 发送GET请求
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Failed to send request: %s\n", curl_easy_strerror(res));
    }

    // 关闭文件和CURL句柄
    fclose(file);
    curl_easy_cleanup(curl);

    // 清理CURL库
    curl_global_cleanup();
}
void basic_json(char* buf)
{
    root = cJSON_Parse(buf);                         //读入json
    status = cJSON_GetObjectItem(root, "status");    //指向json-status
    result = cJSON_GetObjectItem(root, "result");    //指向json-result

    //总提交数
    num_all_submit = cJSON_GetArraySize(result);
    //printf("num all submit = %d\n", num_all_submit);

    //不同标签数
    int tag_size = 0;

    //提交记录中最晚年份
    array_in_result = cJSON_GetArrayItem(result, 0);
    time_in_array = cJSON_GetObjectItem(array_in_result, "creationTimeSeconds");
    time_temp = (unsigned long)cJSON_GetNumberValue(time_in_array);
    biggest_time_info = *localtime(&time_temp);
    biggest_year = biggest_time_info.tm_year;

    //提交记录中最早年份
    array_in_result = cJSON_GetArrayItem(result, num_all_submit - 1);
    time_in_array = cJSON_GetObjectItem(array_in_result, "creationTimeSeconds");
    time_temp = (unsigned long)cJSON_GetNumberValue(time_in_array);
    smallest_time_info = *localtime(&time_temp);
    smallest_year = smallest_time_info.tm_year;

    statistics = (struct statistic*)malloc(sizeof(struct statistic) * (biggest_year - smallest_year + 1));
    //初始化存储统计数据的statistics结构体
    for (int i = 0; i < (biggest_year - smallest_year + 1); i++)
    {
        // 设置结构体成员默认值
        statistics[i].leapyear = false;
        statistics[i].year = 0;

        memset(statistics[i].month, 0, sizeof(statistics[i].month));

        for (int j = 0; j < 12; j++)
        {
            memset(statistics[i].month_day[j], 0, sizeof(statistics[i].month_day[j]));
        }
    }
}
char* string_format(char* src)
{
    int len = strlen(src);
    char* dst = (char*)malloc(sizeof(char) * len - 1);
    //通过复制字符串的长度差，实现字符串截断
    memset(dst, 0, sizeof(char) * len - 1);
    strncpy(dst, src + 1, len - 2);
    return dst;
}
void parse_json(char* handle)
{
    //遍历提交记录
    for (int i = 0; i < num_all_submit; i++)
    {
        array_in_result = cJSON_GetArrayItem(result, i);
        verdict_in_array = cJSON_GetObjectItem(array_in_result, "verdict");

        //若提交状态为'OK'则进入统计
        if (!strcmp(cJSON_Print(verdict_in_array), "\"OK\""))
        {
            time_in_array = cJSON_GetObjectItem(array_in_result, "creationTimeSeconds");
            time_temp = (unsigned long)cJSON_GetNumberValue(time_in_array);
            //printf("%d time = %I64d\n",i,time_temp);

            probs[num_all_ok].time_unix = time_temp;
            probs[num_all_ok].time = *localtime(&time_temp);

            statistics[(probs[num_all_ok].time.tm_year - smallest_year)].year++;
            statistics[(probs[num_all_ok].time.tm_year - smallest_year)].month[probs[num_all_ok].time.tm_mon] ++;
            statistics[(probs[num_all_ok].time.tm_year - smallest_year)].month_day[probs[num_all_ok].time.tm_mon][probs[num_all_ok].time.tm_mday - 1] ++;

            prob_in_array = cJSON_GetObjectItem(array_in_result, "problem");
            tag_in_array = cJSON_GetObjectItem(prob_in_array, "tags");
            int num_tags = cJSON_GetArraySize(tag_in_array);

            rating_in_array = cJSON_GetObjectItem(prob_in_array, "rating");
            int rating = cJSON_GetNumberValue(rating_in_array);
            if (rating <= 1000)
            {
                probs[i].level = Easy;
                easy++;
            }
            else if (rating > 1000 && rating <= 1500)
            {
                probs[i].level = Intermediate;
                intermediate++;
            }
            else if (rating > 1500 && rating <= 2000)
            {
                probs[i].level = Moderate;
                moderate++;
            }
            else if (rating > 2000 && rating <= 2500)
            {
                probs[i].level = Challenging;
                challenging++;
            }
            else if (rating > 2500 && rating <= 3000)
            {
                probs[i].level = Difficult;
                difficult++;
            }
            else
            {
                probs[i].level = Expert;
                expert++;
            }

            num_all_ok++;

            for (int j = 0; j < num_tags; j++)
            {
                cJSON* tag_cjson = nullptr;
                tag_cjson = cJSON_GetArrayItem(tag_in_array, j);
                //probs[tag_size].tag = cJSON_Print(tag_cjson);     
                
                //由于不可操作野指针，决定先写入tag再判断是否存在相同项
                probs_cat[tag_size].tag = string_format(cJSON_Print(tag_cjson));
                //int found = 0;
                //搜索之前储存的所有tag
                for (int k = 0; k <= tag_size; k++)
                {
                    if (!strncmp(probs_cat[tag_size].tag, probs_cat[k].tag, sizeof(probs_cat[k].tag)))
                    {
                        //found = 1;
                        probs_cat[k].num_tag++;
                        //搜索到边界时才相等的话 说明之前无相同的tag
                        if (k == tag_size)  tag_size++;
                        //搜索到相同的标签后，遍历下一条做题记录
                        break;
                    }
                } 
                //if (!found)
                //{
                //    probs_cat[tag_size].tag = (char*)malloc(strlen(probs_cat[tag_size].tag) + 1);
                //    probs_cat[tag_size].num_tag++;
                //    tag_size++;
                //}
            }
        }
    }
}
void create_json(char* handle)
{
    cJSON* response = cJSON_CreateObject();
    cJSON* status_json = cJSON_AddStringToObject(response, "status", string_format(cJSON_Print(status)));
    cJSON* handle_json = cJSON_AddStringToObject(response, "handle", handle);
    cJSON* min_year = cJSON_AddNumberToObject(response, "min_year", smallest_year);
    cJSON* max_year = cJSON_AddNumberToObject(response, "max_year", biggest_year);
    cJSON* year = cJSON_AddArrayToObject(response, "year");
    cJSON* data_tag = cJSON_AddArrayToObject(response, "data_tag");                     //tags
    cJSON* data_all_submitted = cJSON_AddNumberToObject(response, "data_all_submitted", num_all_submit);
    cJSON* data_all_solved = cJSON_AddNumberToObject(response, "data_all_solved", num_all_ok);
    cJSON* group_month_data_per_year = cJSON_AddArrayToObject(response, "group_month_data_per_year");
    cJSON* group_year_data = cJSON_AddArrayToObject(response, "group_year_data");

    for (int i = smallest_year; i <= biggest_year; i++)
    {
        cJSON_AddNumberToObject(year, "year", i + 1900);
    }

    for (int i = 0; i < tag_size; i++)                                                  //输出tag的json
    {
        cJSON* obj_in_data_pie = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj_in_data_pie, "value", probs_cat[i].num_tag);
        cJSON_AddStringToObject(obj_in_data_pie, "name", probs_cat[i].tag);
        cJSON_AddItemToArray(data_tag, obj_in_data_pie);
    }

    cJSON* group_data_heat = cJSON_AddArrayToObject(response, "group_data_heat");         //输出热力图数据的json
    cJSON* year_data_heat = cJSON_CreateArray();
    cJSON* data_heat = cJSON_CreateArray();
    for (int i = num_all_ok - 1, j = smallest_year; i >= 0; i--)
    {
        if (probs[i].time.tm_year != j)
        {
            year_data_heat = cJSON_CreateArray();
            j++;
        }
        data_heat = cJSON_CreateArray();
        char date_data_heat[11] = { 0 };
        sprintf(date_data_heat, "%04d-%02d-%02d", probs[i].time.tm_year + 1900, probs[i].time.tm_mon + 1, probs[i].time.tm_mday);
        cJSON_AddStringToObject(data_heat, "date", date_data_heat);
        cJSON_AddNumberToObject(data_heat, "num", statistics[probs[i].time.tm_year - smallest_year].month_day[probs[i].time.tm_mon][probs[i].time.tm_mday - 1]);
        cJSON_AddItemToArray(year_data_heat, data_heat);
        if (i == 0 || probs[i - 1].time.tm_year != j)
        {
            cJSON_AddItemToArray(group_data_heat, year_data_heat);
        }
        //printf("num = %d : json = %s\n", i, cJSON_Print(group_data_heat));
    }

    for (int i = 0; i < (biggest_year - smallest_year + 1); i++)
    {
        cJSON* month_data_per_year = cJSON_CreateArray();                               //输出有做题记录年份的每月做题数
        for (int j = 0; j < 12; j++)
        {
            cJSON_AddNumberToObject(month_data_per_year, "month_data_per_year", statistics[i].month[j]);
        }

        cJSON_AddItemToArray(group_month_data_per_year, month_data_per_year);

        cJSON_AddNumberToObject(group_year_data, "year_data", statistics[i].year);
    }
    cJSON* data_level = cJSON_AddArrayToObject(response, "data_level");                     //输出题目难度分布的json
    cJSON* obj_in_data_bar = cJSON_CreateObject();
    cJSON_AddNumberToObject(data_level, "easy", easy);
    cJSON_AddNumberToObject(data_level, "intermediate", intermediate);
    cJSON_AddNumberToObject(data_level, "moderate", moderate);
    cJSON_AddNumberToObject(data_level, "challenging", challenging);
    cJSON_AddNumberToObject(data_level, "difficult", difficult);
    cJSON_AddNumberToObject(data_level, "expert", expert);

    char* temp = cJSON_Print(response);
    printf("%s", string_format(cJSON_Print(status)));

    FILE* response_parsed = fopen("response_parsed.json", "w");
    fwrite(temp, strlen(temp), 1, response_parsed);
}