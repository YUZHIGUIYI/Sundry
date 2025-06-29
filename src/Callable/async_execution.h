//
// Created by ZZK on 2025/4/6.
//

#pragma once

#include <type_traits>
#include <memory>
#include <utility>
#include <optional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

struct immovable {
    immovable() = default;
    immovable(immovable &&) = delete;
};

// 简单萃取connect()的返回类型
template <typename S, typename R>
using connect_result_t = decltype(connect(std::declval<S>(), std::declval<R>()));

// 简单萃取sender<result_t>模板中的result_t
template <typename S>
using sender_result_t = typename S::result_t;

// connect()后得到的产物
template <class R, class T>
struct just_operation : immovable {
    R rec;
    T value;

    // operation需要实现start()
    // 由于just()操作是start()后立刻完成的，只需直接通知receiver
    friend void start(just_operation& self) {
        // 通过调用set_value()实现完成通知
        set_value(self.rec, self.value);
    }
};

// 2. just sender存放value
template <class T>
struct just_sender {
    using result_t = T;
    T value;

    // sender需要实现connect()，返回operation state
    template <class R>
    friend just_operation<R, T> connect(just_sender self, R rec) {
        return { {}, rec, self.value };
    }
};

// 要实现的sender factory接口，接受一个值，传递到sender然后立刻完成
template <class T>
just_sender<T> just(T t) {
    return {t};
}

//
// start test code
//
struct cout_receiver {
    friend void set_value(cout_receiver self, auto val) {
        // 这个receiver什么都不干，只对收集到的结果输出
        std::cout << "Result: " << val << '\n';
    }

    friend void set_error(cout_receiver self, std::exception_ptr err) {
        std::terminate();
    }

    friend void set_stopped(cout_receiver self) {
        std::terminate();
    }
};

///////////////////////////////////////////
// then(Sender, Function) sender adaptor
///////////////////////////////////////////

// 4.
template <class R, class F>
struct then_receiver {
    R rec;
    F f;

    // 当通知完成时，再将收集的值val转换为f(val)
    friend void set_value(then_receiver self, auto val) {
        set_value(self.rec, self.f(val));
    }

    friend void set_error(then_receiver self, std::exception_ptr err) {
        set_error(self.rec, err);
    }

    friend void set_stopped(then_receiver self) {
        set_stopped(self.rec);
    }
};

// 3.
template <class S, class R, class F>
struct then_operation : immovable {
    // S为内部sender
    // 这里需要实现then_receiver
    connect_result_t<S, then_receiver<R, F>> op;

    friend void start(then_operation& self) {
        // 先执行内部start，就能启动链式操作
        start(self.op);
        // 实际后面也没事情干了
    }
};

// 2. 现在要实现then()返回的then_sender
template <class S, class F>
struct then_sender {
    using result_t = std::invoke_result_t<F, sender_result_t<S>>;
    // 需要暂存then()传递过来的内部sender和function
    S s;
    // function需要内部sender产生的value才能传递下去
    // 并且f需要传递给receiver
    F f;

    template <class R>
    friend then_operation<S, R, F> connect(then_sender self, R rec) {
        // connect()操作先完成内部sender与then_receiver的connect()
        // 这里需要实现一个then_operation和then_receiver
        // NOTE:
        // 虽然operation是immovable，但是C++17有guaranteed copy elision
        // 因此返回是没有问题
        return { {}, connect(self.s, then_receiver<R, F>{rec, self.f}) };
    }
};

// 1. 要实现的then()接口
// 需要返回then_sender
template <class S, class F>
then_sender<S, F> then(S s, F f) {
    // 简单的构造sender
    return {s, f};
}

//////////////////////////////////////////
// sync_wait() sender consumer
///////////////////////////////////////////

// 2. 考虑到异步操作可以在不同的线程间执行
// 因此需要一个control block
struct sync_wait_data {
    std::mutex mtx;
    std::condition_variable cv;
    std::exception_ptr err;
    bool done = false;
};

// 3. 需要一个receiver来实现value收集和通知
template <class T>
struct sync_wait_receiver {
    sync_wait_data& data;
    std::optional<T>& value;

    friend void set_value(sync_wait_receiver self, auto val) {
        std::unique_lock lk{self.data.mtx};
        self.value.emplace(val);
        self.data.done = true;
        self.data.cv.notify_one();
    }

    friend void set_error(sync_wait_receiver self, std::exception_ptr err) {
        std::unique_lock lk{self.data.mtx};
        self.data.err = err;
        self.data.done = true;
        self.data.cv.notify_one();
    }

    friend void set_stopped(sync_wait_receiver self) {
        std::unique_lock lk{self.data.mtx};
        self.data.done = true;
        self.data.cv.notify_one();
    }
};

// 1. 从前面的例子可看到，sync_wait接收一个任务，即sender
template <class S>
std::optional<sender_result_t<S>> sync_wait(S s) {
    // 通过前面2份代码可以看出，sender一般会实现result_t方便萃取
    using T = sender_result_t<S>;
    // 需要一个control block来保证线程安全（见2.）
    sync_wait_data data;
    std::optional<T> value;

    // 接下来的问题就是如何得到value

    auto op = connect(s, sync_wait_receiver<T>{data, value});
    start(op);

    // 提供一个同步点，在sync_wait_receiver对应的set_value处唤醒（见3.）
    std::unique_lock lk{data.mtx};
    data.cv.wait(lk, [&]{return data.done;});

    if (data.err)
        std::rethrow_exception(data.err);

    return value;
}

///////////////////////////////////////////
// run_loop execution context
///////////////////////////////////////////
struct run_loop : immovable {
    struct none{};

    // 用于支持operation泛型擦除，从而放到链表上
    struct task : immovable {
        task* next = this;
        virtual void execute() {}
    };

    // 支持绑定不同receiver的operation
    template <class R>
    struct operation : task {
        R rec;
        run_loop& loop;

        operation(R rec, run_loop& loop)
          : rec(rec), loop(loop) {}

        void execute() override final {
            // 为了简化示例，value都用none来代替
            set_value(rec, none{});
        }

        // operation需要支持start()
        // 这里就是插入链表
        friend void start(operation& self) {
            self.loop.push_back(&self);
        }
    };

    // 一个FIFO链表
    // 作者提到sender/receiver的设计是避免了动态分配的可能
    // 因此使用的是侵入式链表
    // 你可以看到实际task/operation是在main()函数栈上分配的
    task head;
    // 使用循环链表，让代码更加简洁
    task* tail = &head;
    bool finishing = false;
    // 保证线程安全
    std::mutex mtx;
    std::condition_variable cv;

    void push_back(task* op) {
        std::unique_lock lk(mtx);
        op->next = &head;
        tail = tail->next = op;
        cv.notify_one();
    }

    task* pop_front() {
        std::unique_lock lk(mtx);
        cv.wait(lk, [this]{return head.next != &head || finishing;});
        if (head.next == &head)
            return nullptr;
        return std::exchange(head.next, head.next->next);
    }

    struct sender {
        using result_t = none;
        run_loop* loop;

        template <class R>
        friend operation<R> connect(sender self, R rec) {
            return {rec, *self.loop};
        }
    };

    // 用于指向一个上下文
    struct scheduler {
        run_loop* loop;
        // schedule()生成一个关联上下文的sender
        friend sender schedule(scheduler self) {
            return {self.loop};
        }
    };

    scheduler get_scheduler() {
        return {this};
    }

    // 执行上下文位于run()调用者所在的线程
    void run() {
        while (auto* op = pop_front())
            op->execute();
    }

    // 表示未来不会再加入任何task
    void finish() {
        std::unique_lock lk(mtx);
        finishing = true;
        cv.notify_all();
    }
};

///////////////////////////////////////////
// thread_context execution context
///////////////////////////////////////////
class thread_context : run_loop {
    std::thread th{[this]{ run(); }};
public:
    using run_loop::get_scheduler;
    using run_loop::finish;
    void join() {
        th.join();
    }
};






























