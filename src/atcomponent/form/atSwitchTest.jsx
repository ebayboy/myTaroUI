
import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtSwitch } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtSwitchTest extends Component {
    constructor() {
        super(...arguments)

        this.state = {
            title: '已开启',
            checked: true,
            disabled: false,
            border: true
        }
    }

    handleChange(value) {
        if (value === true) {
            this.setState({title:'已开启', checked:true})
            console.log("checked:", value, '已开启')
        } else {
            this.setState({title:'已关闭', checked:false})
            console.log("checked:", value, '已关闭')
        }
    }

    render() {
        return (
            <View>
                <AtSwitch
                    title={this.state.title}
                    checked={this.state.checked}
                    disabled={this.state.disabled}
                    border={this.state.border}
                    onChange = {this.handleChange.bind(this)}
                ></AtSwitch>
            </View>
        )
    }
}