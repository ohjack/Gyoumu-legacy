<?PHP
/**
 * example that fetches all transactions for an item
 *
 * $Id: example_AddDispute.php,v 1.1 2004/10/28 17:14:53 schst Exp $
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

$disputeId = $dispute = $ebay->AddDispute('4501333179', '0', 2, 5);

echo 'DistputeId: '.$disputeId;
?>