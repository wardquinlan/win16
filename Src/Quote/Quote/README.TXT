Version 1.11.03
---------------

- Trailing Stop Function always uses the close value, even
  if the underlying rule is 'buy/sell on next open'

- Update About dialog

- New function: @yday()

- Function name changes.  The new function names are:

  @abs()   - Absolute Value
  @aroc()  - Arithmetic Rate of Change
  @avg()   - Simple Moving Average
  @bsi()   - Bars Since Entry
  @corr()  - Correlation Coefficient
  @etp()   - Entry Price
  @exp()   - Exponential
  @groc()  - Geometric Rate of Change
  @hgst()  - Highest Value
  @log()   - Natural Logarithm
  @lrg()   - Linear Regression
  @lwst()  - Lowest Value
  @max()   - Maximum Value
  @mday()  - Day of Month
  @min()   - Minimum Value
  @mms()   - Money Management Stop
  @mth()   - Month
  @nmax()  - New Maximum
  @nmin()  - New Minimum
  @ofst()  - Offset
  @pts()   - Number of points
  @shft()  - Shift Function
  @std()   - Standard Deviation
  @trs()   - Trailing Stop
  @wday()  - Day of Week
  @xavg()  - Exponential Moving Average
  @yday()  - Day of Year
  @year()  - Year



Todo:

- Incorporate constants into horizontal lines of all indicator types
    - What would be required would be to feed horizontal lines right
      through the parser, as that is where constants are translated


Version 1.11.04
---------------

- Function name changes/additions:

  @abs()   - Absolute Value
  @aroc()  - Arithmetic Rate of Change
  @avg()   - Simple Moving Average
  @bsi()   - Bars Since Entry
  @corr()  - Correlation Coefficient
  @etp()   - Entry Price
  @exp()   - Exponential
  @groc()  - Geometric Rate of Change
  @hgst()  - Highest Value
  @log()   - Natural Logarithm
  @lrg()   - Linear Regression
  @lwst()  - Lowest Value
  @max()   - Maximum Value
  @mday()  - Day of Month
  @min()   - Minimum Value
  @mms()   - Money Management Stop
  @mth()   - Month
  @nmax()  - New Maximum
  @nmin()  - New Minimum
  @ofs()   - Offset
  @pts()   - Number of points
  @rsi()   - RSI
  @sar()   - SAR
  @shft()  - Shift Function
  @std()   - Standard Deviation
  @trs()   - Trailing Stop
  @vi()    - Volatility Index
  @wday()  - Day of Week
  @xavg()  - Exponential Moving Average
  @yday()  - Day of Year
  @year()  - Year

- Ability to sort performance results based on number of trades 'N'

- Change grid numbers to RHS


Version 1.11.05
---------------

- Remove my name from About box

X Bug Fix: -3 is parsed as "tkMinus, tkFloat{3}"
  - Should be parsed as tkFloat{-3}
  - MUCH TOO HARD !!

- Introduce new @pow() function

- @pow() should accept a constant

- @log() should accept a constant

- @exp() should accept a constant

- @abs() should accept a constant

- rename @ofs() to @offs()

