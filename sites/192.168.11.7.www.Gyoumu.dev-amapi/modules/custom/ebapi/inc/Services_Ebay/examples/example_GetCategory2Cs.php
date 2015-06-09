<?PHP
/**
 * example that fetches categories
 *
 * $Id: example_GetCategory2Cs.php,v 1.1 2005/01/04 20:10:18 schst Exp $
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

$cats = $ebay->GetCategory2CS();
?>