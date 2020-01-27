import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtForm, AtInput, AtButton } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

const INIT_STATE = {
    name: '',
    age: 0,
    passwd: '',
    idcard: '',
    digit: 0.01,
    phone: '',
    notedit: 'not edit',
    cleartext: 'clear text',
    value1: '',
    value2: '',
    value3: '',
    value4: '',
    value5: '',
    value6: '',
    value11: '',
    value12: '',
    value13: '',
    vlaue14: ''
}

export default class AtInputTest extends Component {
    constructor() {
        super(...arguments)

        this.state = {
            ...INIT_STATE
        }
    }

    handleClickSubmit(event) {
        console.log("state:", this.state, "passLen:", this.state.passwd.length)
    }

    handleClickReset(event) {
        this.setState({ ...INIT_STATE })
    }

    /*
    handleChange(type, value, event) {

        console.log("value:", value, "type:", type, "event:", event)
        if (type === 'name') {
            this.setState({ name: value })
        } else if (type === 'age') {
            this.setState({ age: value })
        } else if (type === 'passwd') {
            this.setState({ passwd: value })
        } else if (type === 'idcard') {
            this.setState({ idcard: value })
        } else if (type === 'digit') {
            this.setState({ digit: value })
        } else if (type === 'phone') {
            this.setState({ phone: value })
        } else if (type === 'text') {
            this.setState({ age: value })
        }

        console.log('change state:', this.state)
    }
    */

    handleChange(value) {
        this.setState({
            value
        })
        // 在小程序中，如果想改变 value 的值，需要 `return value` 从而改变输入框的当前值
        console.log("value:", value)
        return value
    }

    render() {
        return (
            <View>
                <AtForm
                    onReset={this.handleClickReset.bind(this)}
                    onSubmit={this.handleClickSubmit.bind(this)}
                >
                    <View>一般用法</View>
                    <AtInput
                        clear
                        name='digit'
                        title='小数'
                        type='digit'
                        placeholder='请输入小数'
                        value={this.state.digit}
                        onChange={this.handleChange.bind(this)}
                    />
                    <AtInput
                        name='value1'
                        title='文本'
                        type='text'
                        placeholder='单行文本'
                        value={this.state.value1}
                        onChange={this.handleChange.bind(this)}
                    />
                    <AtInput
                        name='value2'
                        title='数字'
                        type='number'
                        placeholder='请输入数字'
                        value={this.state.value2}
                        onChange={this.handleChange.bind(this)}
                    />
                    <AtInput
                        name='value3'
                        title='密码'
                        type='password'
                        placeholder='密码不能少于10位数'
                        value={this.state.value3}
                        onChange={this.handleChange.bind(this)}
                    />
                    <AtInput
                        name='value4'
                        title='身份证'
                        type='idcard'
                        placeholder='身份证号码'
                        value={this.state.value4}
                        onChange={this.handleChange.bind(this)}
                    />
                    <AtInput
                        name='value5'
                        title='小数'
                        type='digit'
                        placeholder='请输入小数'
                        value={this.state.value5}
                        onChange={this.handleChange.bind(this)}
                    />
                    <AtInput
                        name='value6'
                        border={false}
                        title='手机号码'
                        type='phone'
                        placeholder='手机号码'
                        value={this.state.value6}
                        onChange={this.handleChange.bind(this)}
                    />

                    <AtInput
                        disabled
                        title='禁用'
                        type='text'
                        placeholder='禁止输入'
                        value={this.state.value11}
                        onChange={this.handleChange.bind(this)}
                    />
                    <AtInput
                        error
                        title='出现错误'
                        type='text'
                        placeholder='点击按钮触发回调'
                        value={this.state.value12}
                        onChange={this.handleChange}
                        onErrorClick={this.handleChange.bind(this)}
                    />
                    <AtInput
                        editable={false}
                        title='不可编辑'
                        type='text'
                        placeholder='不可编辑'
                        value={this.state.value13}
                        onChange={this.handleChange.bind(this)}
                    />
                    <AtInput
                        clear
                        border={false}
                        title='清除按钮'
                        placeholder='点击清除按钮清空内容'
                        type='text'
                        value={this.state.value14}
                        onChange={this.handleChange.bind(this)}
                    />
                    <AtInput
                        clear
                        title='验证码'
                        type='text'
                        maxLength='4'
                        placeholder='验证码'
                        value={this.state.value}
                        onChange={this.handleChange.bind(this)}
                    >
                        <Image src='https://aotu.io/img.png' />
                    </AtInput>
                    <AtButton formType='submit' >input submit</AtButton>
                    <AtButton formType='reset' >input reset</AtButton>
                </AtForm>
            </View>
        )
    }
}