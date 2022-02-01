// SPDX-License-Identifier: MIT

pragma solidity ^0.8.7;


contract Certificate {
    struct certifData{
        string infos;
        address witness;
        address certifier;
    }

    mapping (address => certifData[]) certifPool;

    function create_contract(address _user, address _certifier, string memory _infos) public {
        // Create a validation by a contract for allowed certifiers.
        require(_user != msg.sender, "not valid");
        require(_user != _certifier, "not valid");
        require(msg.sender != _certifier, "not valid");
        certifPool[_user].push(certifData(_infos, msg.sender, _certifier));
    }

    function get_certif_user(address _user) public view returns(certifData[] memory){
        return certifPool[_user];
    }
}
