<?php

class Services_Ebay_Call_GetSessionID extends Services_Ebay_Call
{
   /**
    * verb of the API call
    *
    * @var  string
    */
    protected $verb = 'GetSessionID';

    protected $authType = Services_Ebay::AUTH_TYPE_RUNAME;

   /**
    * parameter map that is used, when scalar parameters are passed
    *
    * @var  array
    */
    protected $paramMap = array(
      'RuName'
    );

  /**
    * make the API call
    *
    * @param    object Services_Ebay_Session
    * @return   string
    */
    public function call(Services_Ebay_Session $session)
    {
      $return = parent::call($session);
      return $return;
    }
}
