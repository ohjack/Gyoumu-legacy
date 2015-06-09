<?PHP
/**
 * example that fetches an item
 *
 * $Id: example_ReviseItem.php,v 1.2 2004/11/07 12:48:52 schst Exp $
 *
 * @package     Services_Ebay
 * @subpackage  Examples
 * @author      Stephan Schmidt
 */
error_reporting(E_ALL);
require_once '../Ebay.php';
require_once 'config.php';

$session = Services_Ebay::getSession($devId, $appId, $certId);
$session->setToken($token);
$ebay = new Services_Ebay($session);

$item = Services_Ebay::loadModel('Item', '4501336830', $session);
$item->Get();

$item->Title = 'Supergirl\'s bra';
$ebay->ReviseItem($item);

/*
You may also use the Revise() method
directly on the Item object

$item->Revise();
*/ 
?>