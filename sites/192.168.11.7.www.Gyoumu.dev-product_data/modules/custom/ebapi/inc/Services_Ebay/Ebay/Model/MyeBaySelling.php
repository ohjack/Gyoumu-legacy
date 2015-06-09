<?PHP
/**
 * Model for My eBay
 *
 * @package Services_Ebay
 * @author  Stephan Schmidt <schst@php.net>
 * @todo    implement some helper methods to work with the lists
 */
class Services_Ebay_Model_MyeBaySelling extends Services_Ebay_Model implements IteratorAggregate
{
   /**
    * available item lists
    *
    * @var  array
    */
    private $resp;

   /**
    * create a new item list
    *
    * @param    array   properties
    * @param    object Services_Ebay_Session
    */
    public function __construct($props, $session = null)
    {
      $this->resp = $props;
    }

   /**
    * get the iterator for the items in the list
    *
    * @return   object
    */
    public function getIterator()
    {
      return new ArrayObject($this->resp);
    }

  public function getResp()
  {
    return $this->resp;
  }
}
?>