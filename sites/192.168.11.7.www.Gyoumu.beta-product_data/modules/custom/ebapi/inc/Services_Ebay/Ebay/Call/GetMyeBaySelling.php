<?php

class Services_Ebay_Call_GetMyeBaySelling extends Services_Ebay_Call
{
   /**
    * verb of the API call
    *
    * @var  string
    */
    protected $verb = 'GetMyeBaySelling';

   /**
    * parameter map that is used, when scalar parameters are passed
    *
    * @var  array
    */
    protected $paramMap = array();

  /**
    * make the API call
    *
    * @param    object Services_Ebay_Session
    * @return   string
    */
    public function call(Services_Ebay_Session $session)
    {
        $return = parent::call($session);
        return Services_Ebay::loadModel('MyeBaySelling', $return, $session);
    }
}
