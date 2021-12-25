//
//  Libcurl_data.cpp
//  FRE6883_FinalProject
//
//  Created by Team_5 on 2021/12/10.
//

#include "Libcurl_data.hpp"
#include "Stock.hpp"
#define THREAD_NUM 6
deque<Stock*> stockdeque;
mutex mu;
using namespace std;

void* myrealloc(void* ptr, size_t size)
{
    if (ptr) return realloc(ptr, size); // if have ptr, resize it, else create a memory with size
    else return malloc(size);
}

size_t write_data(void* ptr, size_t size, size_t nmemb, void* data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)data;
    mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory)
    {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}


map<string,double> fetch_benchmark_price (Stock* benchmark)
{
    // declaration of a pointer to an curl object
    CURL* handle;
    CURLcode result;

    // set up the program environment that libcurl needs
    curl_global_init(CURL_GLOBAL_ALL);

    // curl_easy_init() returns a CURL easy handle
    handle = curl_easy_init();

    // map to store the price of benchmark on each trade date
    map<string, double> benchmark_map;

    // if everything's all right with the easy handle...
    if (handle)
    {
        string url_common = "https://eodhistoricaldata.com/api/eod/";
        string start_date = "2020-10-01";
        string end_date = "2021-11-30";
        string api_token = "619f194e663c33.67428609";
        
        struct MemoryStruct data;
        data.memory = NULL;
        data.size = 0;

        string symbol = benchmark->GetTicker();
        string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
        curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

        //adding a user agent
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

        // perform, then store the expected code in result
        result = curl_easy_perform(handle);

        if (result != CURLE_OK)
        {
            // if errors have occured, tell us what is wrong with result
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        }

        stringstream sData;
        sData.str(data.memory);
        string line;
        string sValue, sDate;
        double dValue;
        vector<string> Date;
        vector<double> Value;
        

        while (getline(sData, line))
        {
            size_t found = line.find('-');
            if (found != std::string::npos)
            {
                sDate = line.substr(0, line.find_first_of(',')); // trade date
                Date.push_back(sDate);
                line.erase(line.find_last_of(','));
                sValue = line.substr(line.find_last_of(',') + 1); // price
                dValue = strtod(sValue.c_str(), NULL); // transform price from string to double
                Value.push_back(dValue); // store prices
                benchmark_map[sDate] = dValue;
            }
        }

        benchmark->SetDates(Date);
        benchmark->SetPrices(Value);
        
        cout<<"Benchmark historical price data fetched"<<endl;
        
        free(data.memory);
        data.size = 0;

    }
    else
    {
        fprintf(stderr, "Curl init failed!\n");
    }

    // cleanup since you've used curl_easy_init
    curl_easy_cleanup(handle);
    // release resources acquired by curl_global_init()
    curl_global_cleanup();

    return benchmark_map;
}


map<string,vector<double>> fetch_stock_price (map<string, Stock*> &stocks, map<string, double> benchmark_map)
{
    // declaration of a pointer to an curl object
    CURL* handle;
    CURLcode result;

    // set up the program environment that libcurl needs
    curl_global_init(CURL_GLOBAL_ALL);

    // curl_easy_init() returns a CURL easy handle
    handle = curl_easy_init();

    // define return
    map<string, vector<double>> stock_price;
    
    time_t startTime,endTime;
    startTime = time(NULL);

    // if everything's all right with the easy handle...
    if (handle)
    {
        string url_common = "https://eodhistoricaldata.com/api/eod/";
        string start_date = "2020-10-01";
        string end_date = "2021-11-30";
        string api_token = "619f194e663c33.67428609";

        map<string, Stock*> ::iterator itr = stocks.begin();
        for (; itr != stocks.end(); itr++)
        {
            struct MemoryStruct data;
            data.memory = NULL;
            data.size = 0;

            string symbol = itr->first;
            string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
            curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

            //adding a user agent
            curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

            // perform, then store the expected code in result
            result = curl_easy_perform(handle);

            if (result != CURLE_OK)
            {
                // if errors have occured, tell us what is wrong with result
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
            }

            stringstream sData;
            sData.str(data.memory);
            string line;
            string sValue, sDate;
            string compDate = itr->second->GetDate();
            double dValue;
            vector<string> Date;
            vector<double> Value;
            vector<double> benchmark_price;
            int count = 0;

            while (getline(sData, line))
            {
                size_t found = line.find('-');
                if (found != std::string::npos)
                {
                    sDate = line.substr(0, line.find_first_of(',')); // trade date
                    Date.push_back(sDate);
                    line.erase(line.find_last_of(','));
                    sValue = line.substr(line.find_last_of(',') + 1); // price
                    dValue = strtod(sValue.c_str(), NULL); // transform price from string to double
                    Value.push_back(dValue); // store prices
                    benchmark_price.push_back(benchmark_map[sDate]);
                    if(sDate < compDate) count += 1;
                }
            }

            itr->second->SetDay0Index(count);
            itr->second->SetDates(Date);
            itr->second->SetPrices(Value);
            itr->second->SetBenchmarkPrices(benchmark_price);

            stock_price[itr->first] = Value;
            
            cout<<itr->second->GetTicker()<<" historical price data fetched"<<endl;
            
            free(data.memory);
            data.size = 0;
        }
        
        endTime = time(NULL);
        double secs = difftime(endTime, startTime);
        cout << endl;
        cout << "Total Run time for single-thread data downloading : " << secs << " seconds" << endl;

    }
    else
    {
        fprintf(stderr, "Curl init failed!\n");
    }

    // cleanup since you've used curl_easy_init
    curl_easy_cleanup(handle);
    // release resources acquired by curl_global_init()
    curl_global_cleanup();

    return stock_price;
}

int thread_block(map<string, double> benchmark_map)
{
    // declaration of a pointer to an curl object
    CURL* handle;
    CURLcode result;

    // curl_easy_init() returns a CURL easy handle
    handle = curl_easy_init();

    // if everything's all right with the easy handle...
    if (handle)
    {
        while(1)
        {
            unique_lock<mutex> unique(mu); // lock to this thread
            if (stockdeque.size() == 0) // once the stockdeque is empty, we can quit the thread
            {
                curl_easy_cleanup(handle);
                unique.unlock(); // unlock and clear this thread
                return 0;
            }
            Stock* tempstock = stockdeque.back(); // extract the stock pointer
            stockdeque.pop_back(); // delete after extracting
            unique.unlock(); // unlock the resources
            
            string url_common = "https://eodhistoricaldata.com/api/eod/";
            string start_date = "2020-10-01";
            string end_date = "2021-11-30";
            string api_token = "619f194e663c33.67428609";
            
            struct MemoryStruct data;
            data.memory = NULL;
            data.size = 0;

            string symbol = tempstock->GetTicker();
            string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
            curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

            //adding a user agent
            curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

            // perform, then store the expected code in result
            result = curl_easy_perform(handle);

            if (result != CURLE_OK)
            {
                // if errors have occured, tell us what is wrong with result
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
            }

            stringstream sData;
            sData.str(data.memory);
            string line;
            string sValue, sDate;
            string compDate = tempstock->GetDate();
            double dValue;
            vector<string> Date;
            vector<double> Value;
            vector<double> benchmark_price;
            int count = 0;

            try // if no error, no catch block is runned
            {
                while (getline(sData, line))
                {
                    size_t found = line.find('-');
                    if (found != std::string::npos)
                    {
                        sDate = line.substr(0, line.find_first_of(',')); // trade date
                        line.erase(line.find_last_of(','));
                        sValue = line.substr(line.find_last_of(',') + 1); // price
                        dValue = strtod(sValue.c_str(), NULL); // transform string price into double
                        Value.push_back(dValue);
                        Date.push_back(sDate);
                        benchmark_price.push_back(benchmark_map[sDate]);
                        if(sDate < compDate) count += 1;
                    }
                }
                
                tempstock->SetDay0Index(count);
                tempstock->SetDates(Date);
                tempstock->SetPrices(Value);
                tempstock->SetBenchmarkPrices(benchmark_price);
                unique.lock();
                cout<<tempstock->GetTicker()<<" historical price data fetched"<<endl;
                unique.unlock();

                
            }
            catch (out_of_range& exc)
            { // we can know exactly which line which file the error is
                cout << exc.what() << " Line:" << __LINE__ << " File:" << __FILE__ << endl;
            }
            catch (...)
            {
                cout << "other error." << " Line:" << __LINE__ << " File:" << __FILE__ << endl;
            }
            
//            unique.lock();
//            cout << symbol << endl;
//            for (int i = 0; i < tempstock->prices.size(); i++) {
//                cout << tempstock->dates[i] << " : " << tempstock->prices[i] << endl;
//            }
//            cout << endl;
//            unique.unlock();
            
            free(data.memory);
            data.size = 0;
        }
    }
    else
    {
        fprintf(stderr, "Curl init failed!\n");
    }
    
    return 0;
}

void fetch_stock_price_multithread(map<string, Stock*>& stocks, map<string, double> benchmark_map)
/*
 Use multi-thread to download data, define the thread block to download a single stock data
 Join all the thread together to realize multi-thread downloading
 */
{
    time_t startTime,endTime;
    startTime = time(NULL);
    
    curl_global_init(CURL_GLOBAL_ALL); // initial the global environment
    
    map<string, Stock*> ::iterator itr = stocks.begin(); // generate the beginning iterator
    for (; itr != stocks.end(); itr++)
    {
        stockdeque.push_back(itr->second); // push all the stock pointer to the deque
    }
    
    thread* threads_block = new thread[THREAD_NUM]; // generate dynamic thread array
    for (int i = 0; i < THREAD_NUM; i++)
    {
        threads_block[i] = thread(thread_block, benchmark_map);  // THREAD_NUM threads for function thread_consumer
    }
    for (int i = 0; i < THREAD_NUM; i++)
    {
        threads_block[i].join(); // join all the thread, join with the main thread to run sequentially
    }
    curl_global_cleanup(); // clean up the environment
    
    endTime = time(NULL);
    double secs = difftime(endTime, startTime);
    cout << endl;
    cout << "Total Run time for multi-thread data downloading : " << secs << " seconds" << endl;
}

void datasanitycheck(map<string, Stock*>& stocks)
{
    int fail_num = 0;
    long size_prices, size_dates;
    map<string, Stock*> ::iterator itr = stocks.begin();
    cout << "Total stocks number is: " <<stocks.size() << endl;
    for (; itr != stocks.end(); itr++)
    {
        vector<double> temp_vec = itr->second->GetPrices();
        vector<double>::iterator smallest = min_element(begin(temp_vec), end(temp_vec)); // the smallest prices must be greater than zero
        
        size_dates = itr->second->GetDates().size();
        size_prices = itr->second->GetPrices().size(); // size of these two vectors must be equal and greater than zero
        
        /*
         check the dates vector for all dates string contains only two -
         */
        vector<string> trade_date = itr->second->GetDates();
        vector<string>::iterator datesitr = trade_date.begin();
        bool dates_bool = false;
        for (; datesitr != trade_date.end(); datesitr++)
        {
            string line = *datesitr;
            int cunt=0;
            int begin=-1;
            while((begin=(int)line.find("-",begin+1)) != string::npos) //count "-" in line
            {
                cunt++;
                begin=begin + 1;
            }
            if (cunt != 2) {
                dates_bool = true;
                break;
            }
        }
        
        
        if ((size_dates!=size_prices) or (size_dates==0) or (*smallest <= 0) or dates_bool)
        {
            fail_num += 1;
        }
    }
    
    if (fail_num == 0) {
        cout << "Data sanity check succeed, all the stocks successfully download data." << endl;
    }
    else {
        cout << "Data sanity check fail, fail number is " << fail_num << endl;
    }
}

