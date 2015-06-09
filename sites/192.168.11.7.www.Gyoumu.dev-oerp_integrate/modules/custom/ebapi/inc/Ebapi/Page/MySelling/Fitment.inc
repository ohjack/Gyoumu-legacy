<?php
namespace Ebapi\Page\MySelling;

class Fitment extends _Abstract
{
  protected function getArch()
  {
    return <<<XML
<tree>
  <field name="ItemID"/>
  <field name="Site"/>
  <field name="SKU"/>
  <field name="Title"/>
  <field name="ListingType"/>
  <!--<field name="BuyItNowPrice"/>-->
  <!--<field name="CurrentPrice" string="Cur.Price"/>-->
  <!--<field name="MinimumToBid" string="Min.Bid"/>-->
  <!--<field name="ShippingServiceCost" string="S.Fee"/>-->
  <!--<field name="QuantityAvailable" string="Qty"/>-->
  <!--<field name="Quantity" string="Qty"/>-->
  <!--<field name="StartTime"/>-->
</tree>
XML;
  }

  protected function getAhahPath()
  {
    return 'ebapi/fitment/myselling/js';
  }
}