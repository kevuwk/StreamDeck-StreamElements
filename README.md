Go to https://streamelements.com/dashboard/account/channels
On the right there should be a button to "Show secrets". Click this.
There should be some extra text called "JWT Token". The JWT Token is effectively login credentials 
so do not share these with anybody.

make a note of the "Account ID" and "JWT Token"

------

After installing the plugin there should be a new section in the Stream Deck app called StreamElements. Under this section will be 'Store Item' and 'Close Store'

------
## Store Item

__Store Item__ is used to interact with a specific store item in a specific channel.

Title - Generally shouldn't be used and is replaced by Display Name

Display Name - This should be used instead of Title. If a store item has a restricted number of redemptions then the button will display how many are left under the display name.

Channel - This should be the ID of the channel you want to query. This is the "Account ID" captured previously.

Store Item - This is the name of the store item on streamelements. Choose from the dropdown. (won't populate until channel and JWT are set).

Amount - This is the amount of the item that should be made available when enabled. -1 is unlimited

JWT Token - This is the "JWT Token" captured previously. You only need to input this token once and click the Set button underneath. This token will then be saved within the stream deck and when the stream deck button loses focus within the app the text box will go blank.

Set API Token - This button is used to update the API (JWT) token. If the 'JWI Token' text box above is empty then it will effectively clear what is stored.

------
__Action__

Pressing the button will toggle the status of the store item. If the item is disabled it will become enabled and if enabled will become disabled.

------
__Button Status__

Green - The button will be green if the store item is enabled. If the store item has a restricted number of redemptions then the amount left will also be displayed on the button.

Blue - This button will be blue if the store item is enabled and sold out.

Red - The button will be red if the store item is disabled.

Yellow - The button will be yellow if there has been an issue retrieving data for this item. This could be because the Channel or Store Item data is incorrect

------
## Close Store
__Close Store__ will close all enabled store items for the designated channel that are on the stream deck

Title - You can use the title on this button

Channel - This should be the ID of the channel you want to query. This is the "Account ID" captured previously.

This button will only work properly if you have atleast 1 'Store Item' button for the channel.

------
__Action__

Pressing the button will change every store item on the stream deck to disabled.

------
__Button Status__

Green - The button will be green if there is an enabled store item that is also a button on the stream deck

Red - This button will be red if there are no enabled store items that are also a button on the stream deck. This does not mean there are no enabled store items just that the stream deck isn't monitoring them

------
## Change Store Item Cost
__Change Store Item Cost__ will close all enabled store items for the designated channel that are on the stream deck

Title - Generally shouldn't be used and is replaced by Display Name

Display Name - This should be used instead of Title. The cost will be appended automatically by the plugin.

Channel - This should be the ID of the channel you want to query. This is the "Account ID" captured previously.

Store Item - This is the name of the store item on streamelements. Choose from the dropdown. (won't populate until channel and JWT are set).

Cost 1 - This is the primary cost of the item.

Cost 2 - This is the secondary cost of the item.

JWT Token - This is the "JWT Token" captured previously. You only need to input this token once and click the Set button underneath. This token will then be saved within the stream deck and when the stream deck button loses focus within the app the text box will go blank.

Set API Token - This button is used to update the API (JWT) token. If the 'JWI Token' text box above is empty then it will effectively clear what is stored.

------
__Action__

Pressing the button will switch the cost of the item between cost 1 and cost 2. If the cost matches cost 1 then it will change to cost 2 and vice versa. If the cost doesn't match either number then the cost will be set to cost 1. This will change the cost whether the item is enabled or disabled but will not change this state. This button will not change colour and the only thing that changes is the cost.

------
__Button Status__

Not specific status but the current cost is displayed on the button.
