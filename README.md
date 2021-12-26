# Quarterly-Earnings-surprise-impact-on-Russell-1000-stocks-abnormal-return-movement
Based on the 1 stquarter of 2021 earnings announcement for Russell 1000 stocks (See the Earnings Announcements sheet), sort all the surprise% in ascending order, and split all the stocks into 3 groups with relatively equivalent numbers of stocks:

i. Highest surprise group: Beat Estimate Group

ii. Lowest surprise group: Miss Estimate Group

iii. The rest stocks in between: Meet Estimate Group

## Implement Bootstrapping:

a. Randomly selecting 80 stocks from each group, total 240 stocks.

b. Use libcurl lib to retrieve 2N+1 days of historical prices for Russell 1000 stocks and ETF ticker IWB (used as market benchmark) around the date of earning release (You could enhance our class example for this purpose). N is integer which must be greater or equal to 60, will be entered by users. Users will be warned if there are no enough historical prices for 2N+1.

c. For each stock calculate the daily returns R it for N days before the day “zero” and N days after, such as t = -60, -59,…-1, 0, 1,…, 59, 60:

d. Calculate the corresponding daily return R mt for IWB for the same days.

e. Define abnormal returns as the difference ARit = Rit –Rmt. 

f. Calculate average daily abnormal returns for each group of stocks (with M stocks, M = 50 in our case) for all 2N reference days

g. Cumulate the returns on the first T days to CAAR:

h. Repeat steps a to g 40 times to create 40 samplings and then calculate

- Average AAR and CAAR for 40 samplings for each group

- Standard Deviation of AAR and CAAR for each group

Generate a gnuplot chart show the averaged CAAR of all three groups.
